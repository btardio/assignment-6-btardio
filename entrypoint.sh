#!/bin/bash
#/actions-runner/config.sh --name myrunner --unattended --replace --url https://github.com/cu-ecen-aeld/assignment-1-btardio --token AFAF6AMWUXCTSB3XP5QMG6LHRRR3C
#/actions-runner/run.sh&
git config --global --add safe.directory /repo
git config --global --add safe.directory /repo/assignment-autotest
git config --global --add safe.directory /repo/buildroot


syslog-ng
exec "$@"
