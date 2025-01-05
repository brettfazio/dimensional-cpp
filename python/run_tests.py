import os
import subprocess
import sys

# Paths
clang_tidy = "clang-tidy"
plugin_path = "./libDimensionalAnalysisCheck.dylib"
config_file = "./.clang-tidy"
compile_commands = "./compile_commands.json"

def run_clang_tidy(file_path):
    """Run clang-tidy on a given file and return its output."""
    try:
        result = subprocess.run(
            [
                clang_tidy,
                f"-p={compile_commands}",
                f"--load={plugin_path}",
                f"--config-file={config_file}",
                file_path,
            ],
            capture_output=True,
            text=True,
        )
        return result.stdout + result.stderr
    except FileNotFoundError:
        print(f"Error: {clang_tidy} not found.")
        sys.exit(1)

def main():
    test_dir = "tests"
    results = []

    # Test "right" cases
    right_dir = os.path.join(test_dir, "right")
    for test_file in sorted(os.listdir(right_dir)):
        if test_file.endswith(".cpp"):
            file_path = os.path.join(right_dir, test_file)
            output = run_clang_tidy(file_path)
            if "error:" in output or "warning:" in output:
                results.append((file_path, "FAILED", "Expected no errors but found errors."))
            else:
                results.append((file_path, "PASSED"))

    # Test "wrong" cases
    wrong_dir = os.path.join(test_dir, "wrong")
    for test_file in sorted(os.listdir(wrong_dir)):
        if test_file.endswith(".cpp"):
            file_path = os.path.join(wrong_dir, test_file)
            output = run_clang_tidy(file_path)
            if "error:" in output or "warning:" in output:
                results.append((file_path, "PASSED"))
            else:
                results.append((file_path, "FAILED", "Expected errors but found none."))

    # Print results
    for result in results:
        if len(result) == 3:
            print(f"{result[0]}: {result[1]} ({result[2]})")
        else:
            print(f"{result[0]}: {result[1]}")

    # Exit with failure if any test failed
    if any(r[1] == "FAILED" for r in results):
        sys.exit(1)

if __name__ == "__main__":
    main()
