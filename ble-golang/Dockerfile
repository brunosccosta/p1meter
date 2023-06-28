FROM golang:1.20.5

# Set the working directory inside the container
WORKDIR /app

# Copy the source code into the container
COPY . .

# Build the Go application
RUN go build -o ble-mqtt

# Set the entry point to run the application
ENTRYPOINT ["/app/ble-mqtt"]