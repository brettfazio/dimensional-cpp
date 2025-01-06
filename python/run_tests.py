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
                "tests/helper.hh",
            ],
            capture_output=True,
            text=True,
        )
        return result.stdout + result.stderr
    except FileNotFoundError:
        print(f"Error: {clang_tidy} not found.")
        sys.exit(1)

def analyze_output(output, is_right):
    """
    Analyze the clang-tidy output for correctness:
    - If `is_right` is True, there should be no errors or warnings.
    - If `is_right` is False, there should be plugin-specific errors/warnings, but no "clang-diagnostic-error".
    """
    errors_or_warnings = "error:" in output or "warning:" in output
    clang_diagnostic_error = "clang-diagnostic-error" in output or 'Error: no checks enabled' in output

    if is_right:
        # "Right" cases should have no errors or warnings.
        return not errors_or_warnings, f"Expected no errors or warnings. {output}"
    else:
        # "Wrong" cases should have plugin-specific errors/warnings and no "clang-diagnostic-error".
        if not errors_or_warnings:
            return False, f"Expected errors or warnings but found none. {output}"
        if clang_diagnostic_error:
            return False, f"Unexpected 'clang-diagnostic-error' found. {output}"
        return True, ""

def main():
    test_dir = "tests"
    results = []

    # Test "right" cases
    right_dir = os.path.join(test_dir, "right")
    for test_file in sorted(os.listdir(right_dir)):
        if test_file.endswith("test_no_unit_member.cpp"):
            file_path = os.path.join(right_dir, test_file)
            output = run_clang_tidy(file_path)
            passed, message = analyze_output(output, is_right=True)
            if passed:
                results.append((file_path, "PASSED"))
            else:
                results.append((file_path, "FAILED", message))

    # Test "wrong" cases
    wrong_dir = os.path.join(test_dir, "wrong")
    for test_file in sorted(os.listdir(wrong_dir)):
        if test_file.endswith("test_no_unit_member.cpp"):
            file_path = os.path.join(wrong_dir, test_file)
            output = run_clang_tidy(file_path)
            passed, message = analyze_output(output, is_right=False)
            if passed:
                results.append((file_path, "PASSED"))
            else:
                results.append((file_path, "FAILED", message))

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
