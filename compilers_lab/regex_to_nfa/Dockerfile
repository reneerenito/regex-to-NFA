# Use the latest Alpine image as the base
FROM alpine:latest

# Install build tools (gcc, musl-dev, make, cmake) for compiling the code
RUN apk add --no-cache gcc musl-dev make cmake

# Create a directory for the application
WORKDIR /app

# Copy the source code into the container
COPY . .

# Compiles the code and runs the program with the given flag.
# Default mode is -t and you can pass -r after the image name instead.
ENTRYPOINT ["sh", "-c", "cmake . >/dev/null && make >/dev/null && ./regex_to_nfa \"$@\"", "--"]
CMD ["-t"]
