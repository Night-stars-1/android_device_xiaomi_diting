#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
"""Validate the literal OTA base URL used in product properties."""
import ipaddress
import re
import sys
from urllib.parse import urlsplit


def valid(value):
    try:
        url = urlsplit(value)
        host, port = url.hostname, url.port
        if (url.scheme != 'https' or not host or url.username is not None
                or url.password is not None or url.query or url.fragment
                or '?' in value or '#' in value
                or not re.fullmatch(r'(?:[A-Za-z0-9/._~:@-]|%[0-9A-Fa-f]{2})*', url.path)
                or (port is not None and not 1 <= port <= 65535)):
            return False
        if ':' in host:
            ipaddress.IPv6Address(host)
            authority = '[' + host + ']'
        else:
            if len(host) > 253 or not all(re.fullmatch(r'[A-Za-z0-9](?:[A-Za-z0-9-]{0,61}[A-Za-z0-9])?', part) for part in host.split('.')):
                return False
            authority = host
        return url.netloc.lower() == (authority + (':' + str(port) if port is not None else '')).lower()
    except ValueError:
        return False


if __name__ == '__main__':
    sys.exit(0 if len(sys.argv) == 2 and valid(sys.argv[1]) else 1)
