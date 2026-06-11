import json
import logging
import os

logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")


def reassemble_link(registry_path="/home/samantha/ghost/config/protocol_registry.json"):
    """
    Reads the protocol registry and assembles the Frankinmodule brain map.

    Args:
        registry_path (str): Path to the protocol_registry.json file.

    Returns:
        dict: Brain map with keys 'logic', 'memories', and 'archives'.

    Raises:
        FileNotFoundError: If the registry file does not exist.
        ValueError: If the registry file contains invalid JSON.
        TypeError: If any registry value is not a list.
    """
    if not os.path.exists(registry_path):
        raise FileNotFoundError(
            f"Registry not found at '{registry_path}'. Run the Discovery Engine first."
        )

    with open(registry_path, "r") as f:
        try:
            registry = json.load(f)
        except json.JSONDecodeError as e:
            raise ValueError(f"Malformed registry JSON: {e}")

    # Link categories to Sam's Internal Map
    brain_map = {
        "logic":    registry.get("logic_modules", []),  # Core logic modules
        "memories": registry.get("multimedia", []),      # Maps .jpeg, .mp4 to memory
        "archives": registry.get("archives", []),        # Long-term archive data
    }

    # Validate all values are lists
    for key, val in brain_map.items():
        if not isinstance(val, list):
            raise TypeError(
                f"Expected a list for '{key}', got {type(val).__name__} instead."
            )

    logging.info(
        f"Frankinmodule linked: {len(brain_map['logic'])} logic module(s), "
        f"{len(brain_map['memories'])} memory file(s), "
        f"{len(brain_map['archives'])} archive(s)."
    )

    return brain_map


if __name__ == "__main__":
    try:
        result = reassemble_link()
        print("Brain map assembled successfully:")
        for category, items in result.items():
            print(f"  {category}: {items}")
    except (FileNotFoundError, ValueError, TypeError) as e:
        print(f"Error: {e}")
