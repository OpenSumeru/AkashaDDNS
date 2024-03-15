# AkashaDDNS
A simple DDNS updater using cloudflare API

## Table of Contents

1. [Introduction](#introduction)
2. [Usage](#usage)
3. [Build](#build)
4. [Dependence](#dependence)
5. [Rust Version](#rust-version)

## Introduction

This project is a **simple small** DDNS updater using cloudflare API. 

### Advantages

1. **Small**: This would probably this smallest DDNS updater for both the size of the execution file and the memory footprint.

2. Stable: This project has been run on my test server for months.

### About the development of the project

We try to use [cpp-httplib](https://github.com/yhirose/cpp-httplib) to make a fully C++ based DDNS updater while most DDNS projects are based on Python or Go. **Make C++ Great Again**.

## Usage

### Basic Run

Run this project

```bash
./AkashaDDNS config.json
```

To write config.json, you should replace all 123 in the following json example.

API Key **must include** the accessibility to read and edit the ``API Key Type``. API-Key-Type have two choice ``Auth`` or other else. This depend on whether your api key is for your  or not.

IP Version must be ``IPv4`` or ``IPv6``.

```json
{
    "Header":
    {
        "API-Key-Type":"Zone",
        "API-Key":"123",
        "Email":"test@example.com"
    },
    "Zone":
    {
        "Name":"example.com"
    },
    "Target":
    {
        "DNS-Name":"test.example.com"
    },
    "Setting":
    {
        "IP-Version":"IPv4"
    }
}
```

### Auto Restart

To make AkashaDDNS a service, you can run this code by shell. Please replace /path/to into your path.

```bash
sudo su

cat << EOF > /etc/systemd/system/akashaddns.service
Description=AkashaDDNS Service
After=network-online.target
Wants=network-online.target
[Service]
ExecStart=/path/to/AkashaDDNS /path/to/config.json
Restart=always

[Install]
WantedBy=multi-user.target
EOF

systemctl enable akashaddns
```

To start or stop AkashaDDNS

```bash
sudo systemctl start akashaddns
sudo systemctl stop akashaddns
```

To look up the log of AkashaDDNS

```bash
sudo systemctl status akashaddns
```

## Build

Please Install All [Dependence](#build-dependence) before building.

To build debug version, run

```bash
make AkashaDDNS
```

To build other versions, run

```bash
make AkashaDDNS Version=release
make AkashaDDNS Version=release-min
```

To build all versions, run

```bash
make all
```

## Dependence

### All Third Party Library

[cpp-httplib](https://github.com/yhirose/cpp-httplib)

[nlohmann-json](https://github.com/nlohmann/json)

OpenSSL (3.0+)

### Build Dependence

- [ ] OpenSSL (3.0+) **You should install it** follow the instruction in cpp-httplib
- [ ] [nlohmann-json](https://github.com/nlohmann/json) **You should install it** into your system include path
- [x] [cpp-httplib](https://github.com/yhirose/cpp-httplib)

## Rust Version

[AkashaDDNS-rust](/rust/README.md)
