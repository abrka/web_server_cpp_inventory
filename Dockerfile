# Use a base image with a recent version of Ubuntu or Debian
FROM gcc:latest

EXPOSE 3000

RUN apt-get update && apt-get install -y cmake 

RUN apt-get install -y php

# Set the working directory inside the container
WORKDIR /apps/msg_board

# Copy your project files into the container
COPY . /apps/msg_board

# Create a build directory and run CMake
RUN rm -rf build && mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
RUN cd build && make

WORKDIR /apps/msg_board/build/

# (Optional) If you want to run the built executable directly,
# define a default command to execute when the container starts.
CMD ["./web_server_cpp_message_board"]
