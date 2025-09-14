#!/bin/sh
#
# SPDX-License-Identifier: MIT
#

### BEGIN INIT INFO
# Provides:          aesdsocket
# Required-Start:    $local_fs $network
# Required-Stop:     
# Default-Start:     S
# Default-Stop:      0 6
# Short-Description: Socket server
# Description:       
### END INIT INFO

[ -f /usr/bin/aesdsocket ] && /usr/bin/aesdsocket


