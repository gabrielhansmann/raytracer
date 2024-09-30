
# Raytracer

This project is a simple raytracer built using C++ and OpenGL with GLFW. You can build and run this project both locally using CMake or inside a Docker container for platform-independent execution.

## Prerequisites

### Local Build

- **CMake**: Version 3.10 or newer
- **Build Tools**: `g++`, `make`, etc.
- **Libraries**:
  - `OpenGL`
  - `GLFW`
  - `GLEW`
  - `GLM`

## Building and Running Locally with CMake

1. **Install the required dependencies using the following command (for Ubuntu/Debian)**:

    ```bash
    sudo apt-get update && sudo apt-get install -y \
        build-essential \
        cmake \
        libgl1-mesa-dev \
        libglew-dev \
        libglfw3-dev \
        libglm-dev \
        libxrandr-dev \
        libxinerama-dev \
        libxcursor-dev \
        libxi-dev
    ```

2. **Create a Build Directory**:

   ```bash
   mkdir build && cd build
   ```

3. **Run CMake and Build the Project**:

   ```bash
   cmake ..
   make
   ```

4. **Run the Raytracer**:

   ```bash
   ./raytracer
   ```

## Building and Running with Docker
- Make sure Docker is installed on your system. Follow the installation guide at [https://docs.docker.com/get-docker/](https://docs.docker.com/get-docker/).

### Step 1: Build the Docker Image

To build the Docker image, run the following command in the root of the project directory (where the `Dockerfile` is located):

```bash
sudo docker build -t raytracer:1.0 .
```

### Step 2: Run the Docker Container

For running the raytracer application in Docker and displaying the output on the host's graphical interface, use the following command:

```bash
sudo xhost +local:docker
sudo docker run --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix --name raytracer raytracer:1.0
```

- **`xhost +local:docker`**: This command allows Docker containers to access the host's X server. You can revoke this permission later using `xhost -local:docker`.

## Folder Structure

```
raytracer/
├── build/              # Generated build files
├── src/                # Source files
├── CMakeList.txt       # CMake file
├── Dockerfile          # Dockerfile for building the Docker image
└── README.md           # This readme file
```

## Notes

- After running the Docker container, you can revoke the X server permission using:
  ```bash
  sudo xhost -local:docker
  ```
- The Docker image uses software rendering, so performance may be slower than running directly on hardware.

## License