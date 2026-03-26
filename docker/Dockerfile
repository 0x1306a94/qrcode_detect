FROM ubuntu:22.04
LABEL MAINTAINER=onyxes_accent_0a@icloud.com

WORKDIR /root

ADD ./qrcode_detect.tar.gz /opt/qrcode_detect

EXPOSE 9999

# 安装依赖，zbar需要
RUN apt-get update && apt-get install -y libjpeg-turbo8

RUN ln -s /opt/qrcode_detect/config/qrcode_detect.conf /etc/ld.so.conf.d/qrcode_detect.conf && \
    ldconfig && \
    ln -s /opt/qrcode_detect/bin/qrcode_detect_server /usr/local/bin/qrcode_detect_server && \
    ln -s /opt/qrcode_detect/bin/qrcode_detect_cli /usr/local/bin/qrcode_detect_cli

# ENV PATH=/opt/qrcode_detect/bin:$PATH
# ENV LD_LIBRARY_PATH=/opt/qrcode_detect/lib
ENV LANG=en_US.UTF-8
ENV LC_TIME=en_DK.UTF-8

ENV QRCODE_DETECT_CACHE_DIR=/var/cache/qrcode_detect
ENV DISABLE_CONSOLE_LOG=1

CMD [ "qrcode_detect_server" , "--port", "9999","--model", "/opt/qrcode_detect/models", "--log", "/var/log/qrcode_detect", "--verbose"]
