<!-- PROJECT SHIELDS -->
[![Contributors][contributors-shield]][contributors-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/othneildrew/Best-README-Template">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>
</p>


<!-- TABLE OF CONTENTS -->
## Table of Contents

* [About the Project](#about-the-project)
    * [Input Log File](#input-log-file)
* [Getting Started](#getting-started)
  * [Installation](#installation)
* [Usage](#usage)
    * [Starting the Manager](#starting-the-manager)
    * [User Commands](#user-commands)
* [Testing](#testing)
* [Roadmap](#roadmap)
* [License](#license)
* [Contact](#contact)


<!-- ABOUT THE PROJECT -->
## About The Project

<!-- 
[![Log Manager Screenshot][product-screenshot]](https://github.com/prabd/log_manager)
-->
This project was created to highlight an application of hash tables and to apply optimization techniques wherever possible to achieve better
program performance. 

The Log Manager is a command-line interface making it easy to:
* Perform timestamp, category, or keyword searches
* Manage an excerpt list of the most relevant entries by:
    * Appending search results or a specific log to the list
    * Deleting specific log entry from the list
    * Moving specific log entry to the beginning or end
    * Sorting the list
    * Clearing the list
    * Printing the list

### Input Log File

The Log Manager takes in a input plaintext file containing structured logs and processes it as the user instructs.

Logs are separated by newlines, and log fields are delimited by a vertical bar (|).

Each log has:
* A _timestamp_ in the format `mm:dd:hh:mm:ss`
* Log _categories_ given as a string, with no leading or trailing   whitespace
* Log _messages_ given as a string, with no leading or trailing whitespace

The category and message strings need to always be atleast 1 character long.

A sample log file is shown below:
```sh
...
12:11:20:12:12|TCP#|Bad packet received (CID #8432)
12:15:20:56:23|UI-PANE1|Window received focus.
04:25:21:54:22|Thread|Suspending CPU 3 ui
...
```


<!-- GETTING STARTED -->
## Getting Started
To get a local copy up and running follow these simple example steps.

### Installation

1. Clone the repo
```sh
git clone https://github.com/prabd/log_manager.git
```
2. Navigate into the directory
```sh
cd log_manager/
```
3. Build the executable
```sh
make
```


<!-- USAGE EXAMPLES -->
## Usage

### Help
```sh 
manager -h
manager --help
```

### Starting the Manager
```sh
manager path/to/LOGFILE
```

### User Commands
There are several user commands, split into three general groups: searching the master logfile, editing the excerpt list, and outputting results.

Searching:
* **t - timestamps search**
    * Syntax: `t <timestamp1>|<timestamp2>`
    * Search for log entries between the provided timestamps, inclusive.
    * Requires: timestamp1 < timestamp2, both timestamps formatted as `mm:dd:hh:mm:ss`
* **m - matching timestamp search**
    * Syntax: `m <timestamp>`
    * Search for log entries matching the provided timestamp.
    * Requires: timestamp formatted as `mm:dd:hh:mm:ss`
* **c - category search**
    * Syntax: `c <string>`
    * Search for log entries with categories matching `<string>`
* **k - keyword search**
    * Syntax: `k <string>`
    * Search for log entries that contain the words in `<string>`
    * Requires: words in `<string>` are delimited by non-alphanumeric ASCII characters

Editing Excerpt List:
* **a - append log by entryID**
    * Syntax: `a <integer>`
    * Appends log entry at position `<integer>` in master logfile to end of excerpt list
* **r - append search results**
    * Syntax: `r`
    * Appends previous search results to end of excerpt list
* **d - delete log in excerpt list**
    * Syntax: `d <integer>`
    * Deletes excerpt list entry at position `<integer>`
* **b - move to beginning**
    * Syntax: `b <integer>`
    * Moves excerpt list entry at position `<integer>` to beginning of excerpt list
* **e - move to end**
    * Syntax: `e <integer>`
    * Moves excerpt list entry at position `<integer>` to end of excerpt list
* **s - sort excerpt list**
    * Syntax: `s`
    * Sorts excerpt list by `timestamp`, ties broken by `category`, further ties broken by `entryID`
* **l - clear excerpt list**
    * Syntax: `l` (lowercase L)
    * Clears excerpt list

Output:
* **g - print search results**
    * Syntax: `g`
    * Prints most recent search results
* **p - print excerpt list**
    * Syntax: `p`
    * Prints excerpt list

Other:
* **q - quit**
    * Syntax: `q`
    * Exits the command line interface


<!-- Testing -->
## Testing

To run tests or samples you can run:
```sh
manager LOGFILE < commands.in > output.out
```

### Interactive Example
```sh
manager samples/sample-log.txt
k persevere
g
r
l
```

### Input/Output Redirection Example
```sh
manager samples/sample-log.txt < samples/sample-all-cmds.txt > output.txt
```

<!-- ROADMAP -->
## Roadmap

Possible Future Improvements
* Automate testing
* Add support for multiple categories
* Better error handling when processing logs


<!-- LICENSE -->
## License

Distributed under the MIT License.

<!-- CONTACT -->
## Contact

Prab Dhaliwal - prabd62@gmail.com

Project Link: [https://github.com/prabd/log_manager](https://github.com/prabd/log_manager)

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/othneildrew/Best-README-Template.svg?style=flat-square
[contributors-url]: https://github.com/prabd/log_manager/graphs/contributors
[license-shield]: https://img.shields.io/github/license/othneildrew/Best-README-Template.svg?style=flat-square
[license-url]: https://github.com/prabd/log_manager
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=flat-square&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/prab-dhaliwal-1603531a0
[product-screenshot]: images/screenshot.png