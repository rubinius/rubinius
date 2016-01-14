FROM ubuntu:14.04

ADD https://rubinius-binaries-rubinius-com.s3-us-west-2.amazonaws.com/ubuntu/14.04/x86_64/rubinius-3.7.tar.bz2 /tmp/rubinius.tar.bz2
RUN apt-get -y install bzip2 && cd /opt && tar xvjf /tmp/rubinius.tar.bz2

ENV PATH /opt/rubinius/3.7/bin:/opt/rubinius/3.7/gems/bin:$PATH

CMD ["bash"]
