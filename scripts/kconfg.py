import kconfiglib

def generate_header(kconfig_path, config_file_path, header_file_path):
    """
    Generates a header file from Kconfig and .config settings using Kconfiglib.

    Parameters:
    - kconfig_path: Path to the Kconfig root file.
    - config_file_path: Path to the .config file with configuration settings.
    - header_file_path: Path where the header file will be saved.
    """
    # Load the Kconfig configuration system
    kconf = kconfiglib.Kconfig(kconfig_path)
    # Load the .config file
    kconf.load_config(config_file_path)

    with open(header_file_path, 'w') as header_file:
        header_file.write("#pragma once\n\n")
        
        for sym in kconf.defined_syms:
            # Skip symbols that are not assigned a value
            if sym.str_value == "":
                continue
            else:
                # Handle boolean and tristate symbols
                if sym.type in [kconfiglib.BOOL, kconfiglib.TRISTATE]:
                    value = '1' if sym.str_value != "n" else '0'
                    header_file.write(f"#define CONFIG_{sym.name} {value}\n")
                # Handle other types of symbols
                else:
                    value = sym.str_value
                    # Quote the value if it's a string
                    if sym.type == kconfiglib.STRING:
                        value = f'"{value}"'
                    header_file.write(f"#define CONFIG_{sym.name} {value}\n")

        header_file.write("\n")

    print(f"Header file generated successfully at {header_file_path}")
    
    
generate_header('Kconfig', '.config', 'GENERATED.h')