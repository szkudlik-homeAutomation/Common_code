import sys


def merge_configs(base_config_path, override_config_path, output_config_path):
    # Read base config
    base_config = {}
    with open(base_config_path, 'r') as file:
        for line in file:
            if '=' in line:
                key, value = line.strip().split('=')
                base_config[key] = value

    # Read override config
    override_config = {}
    with open(override_config_path, 'r') as file:
        for line in file:
            if '=' in line:
                key, value = line.strip().split('=')
                override_config[key] = value

    # Merge configs
    merged_config = base_config.copy()
    merged_config.update(override_config)

    # Write merged config
    with open(output_config_path, 'w') as file:
        for key, value in merged_config.items():
            file.write(f'{key}={value}\n')

base = sys.argv[1]
override = sys.argv[2]
merged = sys.argv[3]
merge_configs(base, override, merged)
