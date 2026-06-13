import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Constants
RADIUS = 1.0
SPEED = 2
MAX_REFLECTIONS = 10
FRAME_LIMIT = 1000
FIRE_INTERVAL = 100
CENTER_TOLERANCE = 0.05

# Argon bubble settings
BUBBLE_BASE_RADIUS = 0.05
BUBBLE_SCALE_STEP = 0.33  # Expands to ~3x
FLASH_FRAMES = 16  # 1 second flash
CENTER_HIT_CYCLE = 3

def deg_to_coord(deg):
    rad = np.deg2rad(deg)
    return RADIUS * np.cos(rad), RADIUS * np.sin(rad)

class Wave:
    def __init__(self, source, target, color, reflections=0, split_done=False):
        self.source = source
        self.target = target
        self.color = color
        self.progress = 0
        self.reflections = reflections
        self.split_done = split_done

    def step(self):
        self.progress += SPEED

    def is_complete(self):
        return self.progress >= 100

    def position(self):
        x1, y1 = deg_to_coord(self.source)
        x2, y2 = deg_to_coord(self.target)
        t = self.progress / 100
        return (1 - t) * x1 + t * x2, (1 - t) * y1 + t * y2

def get_reflection_targets(wave):
    if wave.target in [0, 180] and not wave.split_done:
        return [Wave(wave.target, 90, wave.color, wave.reflections + 1, True),
                Wave(wave.target, 270, wave.color, wave.reflections + 1, True)]
    elif wave.target in [90, 270]:
        if wave.source == 0:
            return [Wave(wave.target, 180, wave.color, wave.reflections + 1, True)]
        elif wave.source == 180:
            return [Wave(wave.target, 0, wave.color, wave.reflections + 1, True)]
    elif wave.target == 0 and wave.source in [90, 270]:
        return [Wave(wave.target, 90 if wave.source == 270 else 270, wave.color, wave.reflections + 1, True)]
    elif wave.target == 180 and wave.source in [90, 270]:
        return [Wave(wave.target, 90 if wave.source == 270 else 270, wave.color, wave.reflections + 1, True)]
    return []

# Figure setup with annotated panel
fig, (ax_sim, ax_annotate) = plt.subplots(1, 2, figsize=(12, 6), gridspec_kw={'width_ratios': [2, 1]})
fig.suptitle("Wave-Induced Light Creation Model", fontsize=14)

# Simulation panel
ax_sim.set_aspect('equal')
ax_sim.set_xlim(-1.2, 1.2)
ax_sim.set_ylim(-1.2, 1.2)
ax_sim.set_title("Resonance Chamber")
earth_circle = plt.Circle((0, 0), RADIUS, fill=False, color='lightgray')
argon_bubble = plt.Circle((0, 0), BUBBLE_BASE_RADIUS, color='red', alpha=0.3)
scatter = ax_sim.scatter([], [], s=50)
ax_sim.add_patch(earth_circle)
ax_sim.add_patch(argon_bubble)

# Annotation panel
ax_annotate.axis('off')
explanation = (
    "Emitter Frequencies:\n"
    "  A = 27 kHz (Emitter 1)\n"
    "  B = 27 kHz (Emitter 2)\n"
    "  C = 27 kHz (Harmonic reflection)\n\n"
    "Total Harmonic Product:\n"
    "  A × B × C = 1.9683 × 10¹³ Hz (Infrared spectrum)\n\n"
    "Energy Chain:\n"
    "  Wave Intersections → IR Heat →\n"
    "  H₂O → H₂ + O₂ → Gas Expansion → Ignition\n"
    "  → Blue Light Emission → Collapse → Recycle\n\n"
    "Unified Equation:\n"
    "  E = mv^(ABC)\n\n"
    "Light is an emergent result of\n"
    "structured wave interference,\n"
    "not a fundamental particle.\n"
)
ax_annotate.text(0.01, 0.98, explanation, fontsize=10, verticalalignment='top', family='monospace')

# Simulation variables
wavefronts = []
center_hit_count = 0
bubble_growth_stage = 0
flash_timer = 0
collapse_next = False

def update(frame):
    global wavefronts, center_hit_count, bubble_growth_stage, flash_timer, collapse_next

    x_data, y_data, colors = [], [], []
    new_wavefronts = []

    if frame % FIRE_INTERVAL == 0:
        wavefronts.append(Wave(0, 180, 'yellow'))
        wavefronts.append(Wave(180, 0, 'green'))

    for wf in wavefronts:
        wf.step()
        x, y = wf.position()
        x_data.append(x)
        y_data.append(y)
        colors.append(wf.color)

        if np.hypot(x, y) < CENTER_TOLERANCE and not hasattr(wf, 'counted'):
            center_hit_count += 1
            wf.counted = True
            bubble_growth_stage += 1
            if bubble_growth_stage >= CENTER_HIT_CYCLE:
                flash_timer = FLASH_FRAMES
                collapse_next = True

        if wf.is_complete() and wf.reflections < MAX_REFLECTIONS:
            new_wavefronts.extend(get_reflection_targets(wf))

    wavefronts = [wf for wf in wavefronts if not wf.is_complete()]
    wavefronts.extend(new_wavefronts)

    # Argon bubble behavior
    if flash_timer > 0:
        argon_bubble.set_radius(BUBBLE_BASE_RADIUS * (1 + BUBBLE_SCALE_STEP * CENTER_HIT_CYCLE))
        argon_bubble.set_color('darkblue')
        flash_timer -= 1
    elif collapse_next:
        argon_bubble.set_radius(BUBBLE_BASE_RADIUS)
        argon_bubble.set_color('red')
        bubble_growth_stage = 0
        collapse_next = False
    else:
        scale = 1 + (BUBBLE_SCALE_STEP * bubble_growth_stage)
        argon_bubble.set_radius(BUBBLE_BASE_RADIUS * scale)
        argon_bubble.set_color('red')

    scatter.set_offsets(np.c_[x_data, y_data])
    scatter.set_color(colors)
    return scatter, argon_bubble

ani = animation.FuncAnimation(fig, update, frames=FRAME_LIMIT, interval=60, blit=False)
plt.tight_layout()
plt.show()