# First stage: Build the Go application
FROM golang:alpine AS builder

# Set the working directory to the app directory
WORKDIR /app

# Copy the source code to the app directory
COPY . .

# Build the Go application
RUN go build -o app

# Second stage: Run the Go application
FROM alpine:latest

# Copy the binary from the builder stage to the current directory
COPY --from=builder /app/app .

# Run the binary
CMD ["./app"]
