# AkashaDDNS

**AkashaDDNS** is a Dynamic DNS (DDNS) client written in C++ that supports multiple DNS providers, including Cloudflare and Aliyun. It allows users to keep their domain names updated with their current public IP addresses, ensuring seamless access to services hosted on dynamic IPs.

## Table of Contents

- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage](#usage)
- [Service Management](#service-management)
- [Logging](#logging)
- [Known Issues](#known-issues)
- [Contributing](#contributing)
- [License](#license)

## Features

- **Multi-Provider Support**: Compatible with Cloudflare and Aliyun DNS providers.
- **Configuration Flexibility**: Configure via JSON files or command-line arguments.
- **Robust Logging**: Logs all activities to a file and selectively outputs important messages to the console.
- **Automatic IP Detection**: Retrieves the current public IP using external services.
- **Graceful Shutdown**: Handles system signals for safe termination.
- **Extensible Design**: Easily add support for additional DNS providers.
- **Optional `spdlog` Integration**: Choose between `spdlog` and a custom logging implementation based on build options.
- **Service Management**: Register AkashaDDNS as a system service to run automatically at startup with commands to enable, disable, start, stop, and view logs.

## Prerequisites

- **C++17** or higher
- **xmake** (build system)
- **C++ Libraries**:
  - [cpp-httplib](https://github.com/yhirose/cpp-httplib) (for HTTP requests)
  - [nlohmann/json](https://github.com/nlohmann/json) (for JSON parsing)
  - [cxxopts](https://github.com/jarro2783/cxxopts) (for command-line argument parsing)
  - **Optional:** [spdlog](https://github.com/gabime/spdlog) (for logging)

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

3. **Build the Project with `spdlog` (Default)**:

    ```bash
    xmake
    ```

4. **Build the Project without `spdlog`**:

    ```bash
    xmake f -o --no-use_spdlog
    xmake
    ```

5. **Run the Application**:

    ```bash
    xmake run AkashaDDNS -- --help
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

- `-c, --config`               Configuration file (default: `config.json`)
- `-p, --provider`             DNS Provider (`cloudflare`, `aliyun`)
- `-t, --token`                API Token (for Cloudflare)
- `-a, --access-key-id`        Access Key ID (for Aliyun)
- `-s, --access-key-secret`    Access Key Secret (for Aliyun)
- `-r, --record-name`          Record name to update (e.g., `www.example.com`)
- `-e, --email`                Email address for Cloudflare authentication
- `-h, --help`                 Print usage

**Example**:

```bash
xmake run AkashaDDNS -- --provider cloudflare --record-name www.example.com --email user@example.com --token your_cloudflare_api_token
```

## Usage

Once configured, AkashaDDNS will periodically check your public IP address and update the specified DNS record if a change is detected.

### Running the Application

**With `spdlog` (Default):**

```bash
xmake run AkashaDDNS -- --provider cloudflare --record-name www.example.com --email user@example.com --token your_cloudflare_api_token
```

**Without `spdlog`:**

```bash
xmake run AkashaDDNS -- --provider cloudflare --record-name www.example.com --email user@example.com --token your_cloudflare_api_token
```

**Note:** The usage remains the same; the difference is in how logging is handled internally based on the build option.

### Example Output

**Console** (Only messages containing keywords will be displayed):

```
Startup parameters: provider=Cloudflare, record_name=www.example.com, domain=example.com
IP change detected: old_ip=203.0.113.2, new_ip=203.0.113.3
```

**Log File (`AkashaDDNS.log`)** (All log messages are recorded):

```
[2024-04-27 12:00:00.123] [INFO] Logger initialized successfully.
[2024-04-27 12:00:00.124] [INFO] Startup parameters: provider=Cloudflare, record_name=www.example.com, domain=example.com
[2024-04-27 12:00:00.125] [INFO] Public IP: 203.0.113.1
[2024-04-27 12:00:00.126] [INFO] Country: Exampleland
[2024-04-27 12:00:00.127] [INFO] Region: Example Region
[2024-04-27 12:00:00.128] [INFO] City: Example City
[2024-04-27 12:00:00.129] [INFO] Current DNS IP: 203.0.113.2
[2024-04-27 12:00:00.130] [INFO] IP change detected: old_ip=203.0.113.2, new_ip=203.0.113.3
[2024-04-27 12:00:00.131] [INFO] Successfully updated DNS record for record: www.example.com
[2024-04-27 12:00:00.132] [INFO] Sleeping for 300 seconds...
```

## Service Management

AkashaDDNS can be registered as a system service to run automatically at startup. This allows the application to run continuously in the background without manual intervention.

### Supported Platforms

- **Windows**
- **Linux**

### Commands

The application supports the following service management commands:

- `enable`: Register AkashaDDNS as a service.
- `disable`: Unregister AkashaDDNS from the services.
- `start`: Start the AkashaDDNS service.
- `stop`: Stop the AkashaDDNS service.
- `show`: Display the recent logs of the AkashaDDNS service.

### Usage

**Service Command Structure:**

```bash
AkashaDDNS service [action] [config_file_path (only for enable)]
```

**Actions:**

- `enable`: Requires the path to the configuration file.
- `disable`: No additional arguments.
- `start`: No additional arguments.
- `stop`: No additional arguments.
- `show`: No additional arguments.

### Examples

- **Enable Service:**

  - **Windows** (Run as Administrator):

    ```bash
    AkashaDDNS.exe service enable "C:\path\to\config.json"
    ```

  - **Linux** (Run with sudo):

    ```bash
    sudo ./AkashaDDNS service enable "/path/to/config.json"
    ```

- **Disable Service:**

  - **Windows**:

    ```bash
    AkashaDDNS.exe service disable
    ```

  - **Linux**:

    ```bash
    sudo ./AkashaDDNS service disable
    ```

- **Start Service:**

  - **Windows**:

    ```bash
    AkashaDDNS.exe service start
    ```

  - **Linux**:

    ```bash
    sudo ./AkashaDDNS service start
    ```

- **Stop Service:**

  - **Windows**:

    ```bash
    AkashaDDNS.exe service stop
    ```

  - **Linux**:

    ```bash
    sudo ./AkashaDDNS service stop
    ```

- **Show Recent Logs:**

  - **Windows**:

    ```bash
    AkashaDDNS.exe service show
    ```

  - **Linux**:

    ```bash
    ./AkashaDDNS service show
    ```

### Notes

- **Windows:**
  - **Permissions:** Ensure you run the application with administrative privileges when enabling or disabling services.
  - **Executable Path:** The service registration copies the specified configuration file to the executable's directory as `config.json`. Ensure that the executable has write permissions to its directory.

- **Linux:**
  - **Permissions:** Service-related operations (`enable`, `disable`, `start`, `stop`) require `sudo` privileges.
  - **Service File Location:** Service files are created in `/etc/systemd/system/`.
  - **User and Group:** The service runs under `nobody` user and `nogroup`. Adjust as necessary based on your security requirements.

## Logging

AkashaDDNS utilizes a flexible logging system that can use either `spdlog` or a custom logger based on build configurations.

### With `spdlog`

- **File Logging:** All log messages (`INFO`, `WARN`, `ERROR`) are recorded in `AkashaDDNS.log`.
- **Console Logging:** Only log messages containing specific keywords (e.g., `"IP change"`, `"Startup parameters"`) are displayed in the console.

### With `CustomLogger`

- **File Logging:** All log messages (`INFO`, `WARN`, `ERROR`) are recorded in `AkashaDDNS.log`.
- **Console Logging:** Only log messages containing specific keywords (e.g., `"IP change"`, `"Startup parameters"`) are displayed in the console.

**Note:** The behavior is consistent regardless of the logging implementation, ensuring seamless integration.

## Known Issues

- **Service Functionality Bugs:** The newly added service management features (`enable`, `disable`, `start`, `stop`, `show`) may contain bugs that could prevent them from functioning correctly. Users might experience issues such as the service not starting automatically, failing to register/unregister, or incorrect log outputs. Please use these features with caution and report any issues you encounter.
  
- **Log File Writing in Build Folder:** When building the project using `xmake`, the log file (`AkashaDDNS.log`) may not be written correctly within the build directory. Ensure that the application has the necessary permissions to create and write to log files in the desired directory. As a workaround, consider specifying an absolute path for the log file in the `CustomLogger` configuration or adjust the working directory to a location with appropriate write permissions.

## Contributing

Contributions are welcome! Please follow these steps:

1. **Fork the Repository:** Click the "Fork" button at the top right of this page.
2. **Clone Your Fork:**

    ```bash
    git clone https://github.com/yourusername/AkashaDDNS.git
    cd AkashaDDNS
    ```

3. **Create a Feature Branch:**

    ```bash
    git checkout -b feature/YourFeature
    ```

4. **Commit Your Changes:**

    ```bash
    git commit -m "Add your detailed description of changes"
    ```

5. **Push to Your Fork:**

    ```bash
    git push origin feature/YourFeature
    ```

6. **Open a Pull Request:** Go to the original repository and open a pull request.

## License

This project is licensed under the [MIT License](LICENSE).
