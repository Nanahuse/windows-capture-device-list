# windows-capture-device-list
Python library to list capture-device on windows

## Release

Tag-based GitHub Releases are the only publishing channel (PyPI is not used).

1. Make sure `version` in `pyproject.toml` and `setup.py` match the tag.
2. Create and push a tag:

   ```sh
   git tag vX.Y.Z
   git push origin vX.Y.Z
   ```

3. The `release` workflow verifies the tag matches both static versions, builds and
   verifies `win_amd64` wheels for Python 3.13 and 3.14, then creates a GitHub
   Release and attaches the wheels.
