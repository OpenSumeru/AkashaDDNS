# AkashaDDNS
A simple DDNS updater using cloudflare API

## Introduction

This project is a simple DDNS updater using cloudflare API. We try to use [cpp-httplib](https://github.com/yhirose/cpp-httplib) to make a fully C++ based DDNS updater while most DDNS projects are based on Python or Go.

## Usage

Run this project

```bash
./AkashaDDNS config.json
```


To write config.json, you should replace all 123.

API Key **must include** the accessibility to read and edit the zone. API-Key-Type have two choice "Zone" or other else. This depend on whether your api key is only for the zone or not.

```json
{
    "Headers":
    {
        "API-Key-Type":"Zone",
        "API-Key":"123",
        "Email":"123"
    },
    "Zone":
    {
        "Name":"123"
    },
    "Target":
    {
        "DNS-Name":"123"
    }
}
```

## Build

Run

```bash
make AkashaDDNS
```

## Dependence

### All Third Party Library

[cpp-httplib](https://github.com/yhirose/cpp-httplib)

[json11](https://github.com/dropbox/json11)

OpenSSL (3.0+)

### Build Dependence

- [] OpenSSL (3.0+) **You should install it** follow the instruction in cpp-httplib
- [x] [cpp-httplib](https://github.com/yhirose/cpp-httplib)
- [x] [json11](https://github.com/dropbox/json11)
