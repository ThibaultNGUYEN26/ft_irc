# Run irssi
# docker run -it little_irssi

# Utiliser Alpine Linux comme image de base
# FROM alpine:latest

# Mettre à jour les paquets et installer irssi
# RUN apk update && apk add irssi

# Définir le point d'entrée pour irssi
# ENTRYPOINT ["irssi"]

FROM alpine:latest

RUN apk update && apk add weechat
ENTRYPOINT ["weechat"]


# vim ~./zshrc add docker build -t little_irssi .; alias little_irssi='docker run -it little_irssi'