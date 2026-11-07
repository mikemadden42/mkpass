# mkpass
Make random password

## Usage

```
mkpass length
```

`length` must be a whole number from 12 to 1024. The password is printed to
stdout. Errors and usage go to stderr with exit code 1.

Passwords use the 62 characters `0-9`, `A-Z` and `a-z`, and characters can
repeat. Each character adds about 5.95 bits of strength, so 16 characters is
about 95 bits.

## Building

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build -C Release
```

The `Makefile` also builds `mkpass` without running tests.

## Replace older passwords

Versions before commit `dc07cfd` could only produce about 4.3 billion different
passwords at any length, whatever length was requested. Replace any passwords
made with those versions.
