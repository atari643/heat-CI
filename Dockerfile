FROM ubuntu:22.04

# Installing as root: docker images are usually set up as root.
# Since some autotools scripts might complain about this being unsafe, we set
# FORCE_UNSAFE_CONFIGURE=1 to avoid configure errors.
ENV FORCE_UNSAFE_CONFIGURE=1
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update -y
RUN apt-get install -y build-essential clang cmake curl doxygen ffmpeg ffmpeg2theora gcovr git python-is-python3 python3-numpy python3-matplotlib xsltproc
RUN apt-get autoremove -y
