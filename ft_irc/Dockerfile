# Run irssi
# docker run -it little_irssi

# Utiliser Alpine Linux comme image de base
FROM alpine:latest

# Mettre à jour les paquets et installer irssi
RUN apk update && apk add irssi

# Définir le point d'entrée pour irssi
ENTRYPOINT ["irssi"]

# vim ~./zshrc add alias little_irssi='docker run -it little_irssi'