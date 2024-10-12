# AkashaDDNS

**AkashaDDNS** is a Dynamic DNS (DDNS) client written in C++ that supports multiple DNS providers, including Cloudflare and Aliyun. It allows users to keep their domain names updated with their current public IP addresses, ensuring seamless access to services hosted on dynamic IPs.

## Table of Contents

- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage](#usage)
- [Logging](#logging)
- [Contributing](#contributing)
- [Todo](#todo)
- [License](#license)

## Features

- **Multi-Provider Support**: Compatible with Cloudflare and Aliyun DNS providers.
- **Configuration Flexibility**: Configure via JSON files or command-line arguments.
- **Robust Logging**: Logs all activities to a file and selectively outputs important messages to the console.
- **Automatic IP Detection**: Retrieves the current public IP using external services.
- **Graceful Shutdown**: Handles system signals for safe termination.
- **Extensible Design**: Easily add support for additional DNS providers.

## Prerequisites

- **C++17** or higher
- **CMake** or **xmake** (build system)
- **C++ Libraries**:
  - [spdlog](https://github.com/gabime/spdlog) (for logging)
  - [nlohmann/json](https://github.com/nlohmann/json) (for JSON parsing)
  - [cxxopts](https://github.com/jarro2783/cxxopts) (for command-line argument parsing)
  - [cpp-httplib](https://github.com/yhirose/cpp-httplib) (for HTTP requests)

Ensure that you have these dependencies installed and accessible to your build system.

## Installation

### Using xmake

1. **Clone the Repository**:

    ```bash
    git clone https://github.com/yourusername/AkashaDDNS.git
    cd AkashaDDNS
    ```

2. **Install Dependencies**:

    Ensure that `xmake` is installed on your system. If not, install it from [xmake.io](https://xmake.io/#/guide/installation).

3. **Build the Project**:

    ```bash
    xmake
    ```

4. **Run the Application**:

    ```bash
    xmake run AkashaDDNS --help
    ```

## Configuration

AkashaDDNS can be configured using a JSON configuration file or via command-line arguments. Command-line arguments take precedence over configuration file settings.

### Configuration File

Create a `config.json` file in the project directory with the following structure:

```json
{
    "provider": "cloudflare",
    "token": "your_cloudflare_api_token",
    "access_key_id": "your_aliyun_access_key_id",
    "access_key_secret": "your_aliyun_access_key_secret",
    "record_id": "your_aliyun_record_id",
    "zone_id": "your_aliyun_zone_id",
    "record_name": "www.example.com",
    "email": "user@example.com",
    "update_interval": 300
}
```

**Parameters**:

- `provider`: DNS provider (`cloudflare` or `aliyun`).
- `token`: API token for Cloudflare.
- `access_key_id`: Access Key ID for Aliyun.
- `access_key_secret`: Access Key Secret for Aliyun.
- `record_id`: DNS record ID for Aliyun.
- `zone_id`: DNS zone ID for Aliyun.
- `record_name`: The DNS record to update (e.g., `www.example.com`).
- `email`: Email address for Cloudflare authentication.
- `update_interval`: Time interval (in seconds) between IP updates.

### Command-Line Arguments

```bash
AkashaDDNS [OPTIONS]
```

**Options**:

- `-c, --config`           Configuration file (default: `config.json`)
- `-p, --provider`         DNS Provider (`cloudflare`, `aliyun`)
- `-t, --token`            API Token (for Cloudflare)
- `-a, --access-key-id`    Access Key ID (for Aliyun)
- `-s, --access-key-secret Access Key Secret (for Aliyun)
- `-r, --record-name`      Record name to update (e.g., `www.example.com`)
- `-e, --email`            Email address for Cloudflare authentication
- `-h, --help`             Print usage
```

**Example**:

```bash
xmake run AkashaDDNS -- --provider cloudflare --record-name www.example.com --email user@example.com --token your_cloudflare_api_token
```

## Usage

Once configured, AkashaDDNS will periodically check your public IP address and update the specified DNS record if a change is detected.

### Running the Application

```bash
xmake run AkashaDDNS --provider cloudflare --record-name www.example.com --email user@example.com --token your_cloudflare_api_token
```

### Example Output

**Console**:

```
Startup parameters: provider=Cloudflare, record_name=www.example.com, domain=example.com
IP change detected: old_ip=203.0.113.2, new_ip=203.0.113.3
```

**Log File (`AkashaDDNS.log`)**:

```
[2024-04-27 12:00:00] [info] Logger initialized successfully.
[2024-04-27 12:00:00] [info] Startup parameters: provider=Cloudflare, record_name=www.example.com, domain=example.com
[2024-04-27 12:00:00] [info] Public IP: 203.0.113.1
[2024-04-27 12:00:00] [info] Country: Exampleland
[2024-04-27 12:00:00] [info] Region: Example Region
[2024-04-27 12:00:00] [info] City: Example City
[2024-04-27 12:00:00] [info] Current DNS IP: 203.0.113.2
[2024-04-27 12:00:00] [info] IP change detected: old_ip=203.0.113.2, new_ip=203.0.113.3
[2024-04-27 12:00:00] [info] Successfully updated DNS record for record: www.example.com
[2024-04-27 12:00:00] [info] Sleeping for 300 seconds...
```

## Logging

AkashaDDNS utilizes the `spdlog` library for logging. It maintains a log file named `AkashaDDNS.log` that records all activities. Additionally, it selectively outputs important log messages to the console based on predefined keywords.

### Log Levels

- **Trace**: Detailed information, typically of interest only when diagnosing problems.
- **Debug**: Information useful to developers for debugging the application.
- **Info**: Confirmation that things are working as expected.
- **Warn**: An indication that something unexpected happened, or indicative of some problem in the near future.
- **Error**: Due to a more serious problem, the software has not been able to perform some function.
- **Critical**: A serious error, indicating that the program itself may be unable to continue running.

### Log Filtering

- **File Logging**: All log messages (from `trace` level and above) are recorded in `AkashaDDNS.log`.
- **Console Logging**: Only log messages containing specific keywords (e.g., `"IP change"`, `"Startup parameters"`) are displayed in the console.

## Contributing

Contributions are welcome! Please follow these steps:

1. **Fork the Repository**: Click the "Fork" button at the top right of this page.
2. **Clone Your Fork**:

    ```bash
    git clone https://github.com/yourusername/AkashaDDNS.git
    cd AkashaDDNS
    ```

3. **Create a Feature Branch**:

    ```bash
    git checkout -b feature/YourFeature
    ```

4. **Commit Your Changes**:

    ```bash
    git commit -m "Add your detailed description of changes"
    ```

5. **Push to Your Fork**:

    ```bash
    git push origin feature/YourFeature
    ```

6. **Open a Pull Request**: Go to the original repository and open a pull request.

## Todo

- **Fix Logging Bug**: Currently, there is an issue where the log file (`AkashaDDNS.log`) remains empty despite successful compilation and execution. The problem is related to the `KeywordFilter` class not properly forwarding log messages to the file sink. This needs to be investigated and resolved to ensure all log messages are correctly recorded in the log file.

- **Add Support for Additional DNS Providers**: Extend AkashaDDNS to support more DNS providers beyond Cloudflare and Aliyun.

- **Enhance Error Handling and Retry Mechanisms**: Implement more robust error handling and retry logic for network requests and API interactions.

- **Implement Unit Tests**: Develop comprehensive unit tests to ensure the reliability and stability of all components.

- **Improve Configuration Management**: Enhance the configuration system to support more flexible and secure parameter handling, possibly including encrypted credentials.

- **Optimize Performance**: Explore opportunities to optimize the application's performance, especially in logging and network operations.

## License

This project is licensed under the [MIT License](LICENSE).
