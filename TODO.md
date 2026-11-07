# TODO

Findings from the project review on 2026-11-06. "Tested" means reproduced on
macOS; "by reading" means found by inspecting the code and not run.

**mkpass shouldn't be used for real passwords until the security items are
fixed.** Replace any passwords already generated with it, especially important
ones.

## Fix order

1. Replace the generator in `random_string`.
2. Add strict argument parsing and send errors to stderr.
3. Fix the `Makefile` by pointing it at `main.cc`, or delete it in favor of CMake.
4. Add a few CTest cases for length, repeated characters and rejected input,
   which also fixes the CI step.
5. Clean up the CMake file and the Windows scripts.
6. Document the character set and minimum length in the README.

## Security (`main.cc`)

- [x] **Critical: only about 4.3 billion possible passwords at any length
      (line 15).** `mt19937` is seeded with a single 32-bit value, so a
      16-character password has at most 32 bits of strength, not the roughly 95
      you'd expect.
  - A password generated from a known seed (200,000,000) was found in 63 seconds
    on an 8-core Mac, at about 3.2M seeds/s.
  - A full search of the seed space is estimated at about 22 minutes.
  - A search against a real `mkpass` password hit a 500-second limit after
    covering about 37% of the seeds, so it was not cracked and the 22 minutes
    is an estimate, not a full run.
  - An attacker would need to know the length and the C++ library used, and
    would still have to test each candidate against the target.
- [x] **High: characters never repeat, and length caps out at 62 (lines 17-19).**
      Shuffling and truncating draws without replacement.
  - None of 2,000 sampled 16-character passwords had a repeated character. This
    costs about 3 bits at length 16 and grows with length.
  - Asking for `100` returns 62 characters with no error.
- [x] **High: bad input can silently produce a weak password (lines 28-29).**
      Also add a minimum length, and send errors and usage text to stderr
      instead of stdout.

  | Input      | Result                                | Problem                               |
  |------------|---------------------------------------|---------------------------------------|
  | `3.7`      | 3 characters, exit 0                  | Very short password, no error         |
  | `12abc`    | 12 characters, exit 0                 | Trailing junk ignored                 |
  | `-1`, `-5`, huge values | Crash: uncaught `std::length_error`, exit 134 | Negative number wraps to huge `size_t`; no output, but needs a clean error |
  | `0`        | "Cannot convert argument to number"   | Misleading, as 0 is a number          |

### Suggested fix

This fixes the first two items by drawing each character directly from the OS
random source. It needs `<string_view>` added.

```cpp
std::string random_string(size_t size) {
    static constexpr std::string_view alphabet =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::uniform_int_distribution<size_t> dist(0, alphabet.size() - 1);
    std::string out;
    out.reserve(size);
    for (size_t i = 0; i < size; ++i) out += alphabet[dist(rd)];
    return out;
}
```

For the input handling, parse with `std::from_chars`, reject anything left over,
and enforce a minimum length.

The standard doesn't guarantee `random_device` is non-deterministic, so check
that on each target platform; some old MinGW versions were deterministic.

## Build and CI

### High

- [x] `Makefile:1`: `make` fails with "No rule to make target `mkpass`". Commit
      `24c4bd6` renamed the source to `main.cc` but didn't update the Makefile.
      (Tested)
- [x] `.github/workflows/cmake.yml:36`: `ctest` exits 1 locally with "No test
      configuration file found", because there's no `enable_testing()` or any
      tests. CI itself passed without tests, so this step wasn't failing there
      and likely tested nothing. (Tested locally with ctest 4.4.5; CI result
      from the Actions API)

### Medium

- [x] `run-builds.bat:30`: builds `dirsize.sln`, apparently left over from
      another project, so the MSBuild step can't work. (By reading; script deleted
      in favor of `run-builds.ps1`)
- [x] `run-builds.bat:29,31`: `%errorlevel%` inside a parenthesized block expands
      at parse time, so failures aren't detected. (By reading; script deleted in
      favor of `run-builds.ps1`)
- [x] `run-builds.bash:4`: always exits 1 on macOS, because Apple clang doesn't
      support MSAN. The `.ps1` list also includes sanitizers MSVC
      doesn't support. (Tested on macOS)
- [x] `CMakeLists.txt:12`: `set(CMAKE_CXX_FLAGS ...)` throws away the user's
      flags; `CXXFLAGS=-DFOO` never reached the compile line. Use
      `target_compile_options`. (Tested)

### Low

- [x] `CMakeLists.txt:74-129`: `add_custom_flags` duplicates the per-config flags
      on lines 18-68. Deleting it should simplify the file a lot. (By reading)
- [x] `CMakeLists.txt:122`: the comment assumes `-O2`, but Release actually uses
      `-O3 -DNDEBUG`. (Tested)
- [x] `CMakeLists.txt:38,46`: MSVC gets the TSan and MSan warnings on every
      configure, even for Release. (By reading)
- [x] `CMakeLists.txt:54,107,115`: `/DEBUG /COVERAGE` is passed as one quoted
      argument, and `/COVERAGE` and `/fsanitize=undefined` don't look like valid
      `cl` flags. (By reading, not tested on Windows)
- [x] `Makefile:13`: `make clean` errors if nothing is built; use `rm -f`.
      (Tested)
- [x] `.github/workflows/cmake.yml:18,21`: CI only runs on Ubuntu despite the
      cross-platform scripts, and `checkout@v3` is out of date. (By reading)
