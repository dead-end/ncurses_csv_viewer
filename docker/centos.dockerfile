################################################################################
# File: centos.dockerfile
#
# Build the image:
#
#   sudo docker build -t ccsvv_centos -f docker/centos.dockerfile docker/
#
# Run test:
#
#   docker run -it ccsvv_centos sh /tmp/curses_csv_viewer-master/bin/test_run.sh
################################################################################

FROM centos

#
# The ncurses major version necessary for the config script ncurses5-config or 
# ncurses6-config
#
ARG NCURSES_MAJOR=5

#
# A UTF8 locale is neccessary for unit tests. 'C.utf8' is the only
# installed.
#
ENV LANG=en_GB.utf8

#
# Set path so that test_run.sh finds ccsvv.
#
ENV PATH=$PATH:/tmp/curses_csv_viewer-master

ADD https://github.com/dead-end/curses_csv_viewer/archive/master.zip /tmp

RUN yum -y update && \
	yum -y install unzip && \
	yum -y install gcc && \
	yum -y install make && \
	yum -y install ncurses-devel

#
# Set the directory for the build steps
#
WORKDIR /tmp

#
# Build ccsvv
#
RUN unzip master.zip
        cd curses_csv_viewer-master && \
        make NCURSES_MAJOR=${NCURSES_MAJOR}