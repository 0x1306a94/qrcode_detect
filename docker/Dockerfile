FROM ubuntu:22.04
LABEL MAINTAINER=onyxes_accent_0a@icloud.com

WORKDIR /root

COPY ./cmake_install/qrcode_detect /opt/qrcode_detect

ENV PATH=/opt/qrcode_detect/bin:$PATH
ENV LD_LIBRARY_PATH=/opt/qrcode_detect/lib:$LD_LIBRARY_PATH

EXPOSE 9999

# 安装依赖，zbar需要
RUN apt-get update && apt-get install -y libjpeg-turbo8

CMD [ "qrcode_detect_server" , "--port", "9999","--model", "/opt/qrcode_detect/models", "--log", "/var/log/qrcode_detect", "--verbose"]
