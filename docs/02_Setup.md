# Setup

## Dependencies
- ninja
- or cmake (which ever you prefer)
- clang-tools
- boost
- libllvm
- **catch2_3** (for testing)

## Setup
For the most part the setup should be the same as before.

### CMake
1. On the command line, navigate to the root directory of the project, e.g., `oberon0c`. The root directory of the 
   project is the directory that was created when you cloned the GitLab repository.
2. Create a build directory.
   ```
   > mkdir build
   ```
3. Navigate to the build directory and run CMake to configure the project and generate a native build system.
   ```
   > cd build
   > cmake ..
   ```
4. Call the build system to compile and link the project.
   ```
   > cmake --build .
   ```

### ninja
1. On the command line, navigate to the root directory of the project, e.g., `oberon0c`. The root directory of the 
   project is the directory that was created when you cloned the GitLab repository.
2. Create a build directory.
   ```
   > mkdir build
   ```
3. Navigate to the build directory and run CMake to configure the project and generate a native build system.
   ```
   > cd build
   > cmake .. -G Ninja
   ```
4. Call the build system to compile and link the project.
   ```
   > ninja
   ```
5. Tests can be executed with
   ```
   > ninja test
   ```

### Nix
If you want to play around, a nix flake is provided ([`flake.nix`](../flake.nix)) where all neccessary dependencies are declared. Using `nix develop` will put you into a development environment, while `nix build` should build the entire project.

## Additional Flags
The are no additional flags compared to the original version.
