FROM ubuntu:22.04
RUN apt-get update && apt-get install -y build-essential ca-certificates git wget && rm -rf /var/lib/apt/lists/*
WORKDIR /workspace
COPY . /workspace
RUN make
CMD ["/workspace/lb_sim"]
