FROM ubuntu:24.04


COPY install.sh /install.sh
RUN chmod +x "/install.sh"
RUN apt-get update && apt-get -y install build-essential
RUN apt-get install -y curl
RUN apt-get -y install nasm

# Setting up the user
RUN adduser --disabled-password --gecos '' hal
RUN apt-get install -y sudo
RUN sudo usermod -aG sudo hal
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> \
/etc/sudoers

USER hal
ENTRYPOINT ["sh", "/install.sh"]
