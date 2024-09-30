# Use an official Ubuntu as a base image
FROM ubuntu:20.04

# Set environment variables to avoid user prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Update package lists and install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libgl1-mesa-dev \
    libx11-6 \
    libglew-dev \
    libglfw3-dev \
    libglm-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev

# Set the working directory in the container
WORKDIR /app

# Copy the CMake project into the container
COPY . .

# Create a build directory, run cmake, and build the project
RUN mkdir build && cd build && cmake .. && make

# Specify the command to run your application
CMD ["./build/raytracer"]