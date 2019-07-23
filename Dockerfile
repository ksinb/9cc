FROM ubuntu:18.04

ARG default_user
ARG apt_opts="-qq -y --no-install-recommends"
ARG sudoers_file="80-9cc-default-user"

RUN apt-get -qq update

RUN apt-get $apt_opts install tzdata \
    && ln -sf /usr/share/zoneinfo/Asia/Tokyo /etc/localtime

RUN apt-get $apt_opts install sudo
RUN adduser -q --disabled-password --gecos "" $default_user
RUN gpasswd -a $default_user sudo
RUN echo "# Enable ${default_user} to sudo without password." >> /etc/sudoers.d/$sudoers_file
RUN echo "${default_user} ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers.d/$sudoers_file
RUN chmod o-r /etc/sudoers.d/$sudoers_file

RUN apt-get $apt_opts install build-essential
# RUN apt-get $apt_opts install libc6-dev
RUN apt-get $apt_opts install vim
RUN apt-get $apt_opts install git

COPY startup.sh /startup.sh
RUN chmod 755 /startup.sh
ENTRYPOINT ["/startup.sh"]
