FROM ubuntu:15.10

ADD https://rubinius-binaries-rubinius-com.s3-us-west-2.amazonaws.com/ubuntu/15.10/x86_64/rubinius-3.9.tar.bz2 /tmp/rubinius.tar.bz2
RUN apt-get -y install bzip2 && cd /opt && tar xvjf /tmp/rubinius.tar.bz2

ENV PATH /opt/rubinius/3.9/bin:/opt/rubinius/3.9/gems/bin:$PATH

CMD ["bash"]
