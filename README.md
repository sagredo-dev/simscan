# simscan

Additional information and a guide can be found at
https://www.sagredo.eu/en/qmail-notes-185/installing-and-configuring-simscan-38.html.

Post any issue there.

Overview
--------
SimScan is a simplified scanner for qmail similar to qmail-scanner and qscand.
It uses clamav, trophie, and/or spamassassin.  It also supports attachment
blocking by extension.  Simscan is written entirely in C to ensure maximum
speed.  There are several options to allow simscan to scan per domain, and
reject spam mail.


Requirements
------------
* ripmime (If you plan on using attachment blocking)<br>
* qmail with qmail-queue patch<br>
* clamav (optional)<br>
* spamassassin (optional)<br>
* trophie (or sophie) (both optional)


How it works
---------------
* Simscan creates a temporary working directory. You can specify the base
working directory with the --enable-workdir=/path. The default location
of this base is the  /var/qmail/simscan directory. The temporary working
directory under this bass directory is named as
"unix time in seconds" . "microseconds" . "process id".

The email is read into a temporary file named "msg.unixtime.micro.pid.

* ripmime is called to break this file into separate mime parts.

* Optionally (--enable-attach) the mime message parts are checked against
the list of attachments to block.

Put the list of attachments in /var/qmail/control/ssattach or place the list
in the /var/qmail/control/simcontrol file for per-domain blocking.
This file gets read each time simscan is kicked off.

* clamdscan is called to check all the files in the working directory.
The return code of clamdscan is checked to see if it found a virus.
If it did, simscan exits back to qmail-smtpd with a permenent error,
causing the mail to be bounced back to the sender.

* Optionally (--enable-spam) spam assassin is called via spamc.
The email returned by spamc is checked X-Spam-Flag: header.  If this header
contains "YES", the spam is rejected.  You can reject instead on the calculated
hit count value by adding the --enable-spam-hits.  SimScan checks for both
"hits=" and "score=" so that both old and new versions of spamassassin are
supported.

* Optionally (--enable-per-domain) enable per domain clamav and
spamassassin processing. You will need to edit /var/qmail/control/simcontrol
to define what domains get what scans.  Further details on editing this file
are below.

Then run /var/qmail/bin/simscanmk to build the cdb files that simscan
uses. The changes will take effect immediately.

* Finally, if all of the above succeeds then the msg file is passed on to
qmail-queue. And the working directory and all the temporary files are deleted,
unless the SIMSCAN_DEBUG environment variable is set.

= Configuration Options =
Following is a list of all of the configure options and defaults:

```
  --enable-user=<user>              Change the user for simscan.
                                    Default: simscan.
  --enable-clamav=y|n               Turn on clamav scanning. default yes.
  --enable-clamdscan=PATH           Full path to clamdscan program.
  --enable-custom-smtp-reject=y|n   Return smtp reject message with virus name.
  --enable-per-domain=y|n           Turn on per domain based checking.
  --enable-attach=y|n               Turn on attachment scanning. default no.
  --enable-spam=y|n                 Turn on spam scanning. default no.
  --enable-spam-passthru=y|n        Pass spam email thru or reject
                                    Default: disable (reject)
  --enable-spamc-user=y|n           Set user option to spamc.
  --enable-spam-hits=number         Reject spam above this hit level.
                                    Default 10.0
  --enable-spamc=PATH               Full path to spamc program.
  --enable-spamc-args=ARGS          Arguments to pass to spamc.
  --enable-dropmsg=y|n              Drop message in case of virus/spam found.
                                    Don't return error to sender.
                                    Default: disable (return error)
  --enable-quarantinedir=DIR        Directory to keep spam and/or infected emails
                                    Default: disabled
  --enable-qmaildir=DIR             Base qmail directory /var/qmail.
  --enable-workdir=DIR              Directory to unpack emails
                                    Default: /var/qmail/simscan
  --enable-qmail-queue=PATH         Full path to qmail-queue program.
  --enable-trophie-path=PATH        Full path to the trophie binary.
  --enable-trophie-socket=PATH      Full path to the trophie socket.
  --enable-ripmime=PATH             Full path to ripmime program.
  --enable-received=y|n             Simscan should add a received line showin the version of all scanners that checked the message
```

These options are only needed when the received line option (--enabled-received=y)
and the corresponding scanners are enabled as well:

```
  --enable-spamassassin-path=PATH   Path to the spamassassin binary
  --enable-clamavdb-path=PATH       Directory where the clamav master.cvd and
                                    daily.cvd files are saved
  --enable-sigtool-path=PATH        Path to the sigtool binary
```

Configuration Details
----------------------
Below are more detailed descriptions of each configuration option.

```
--enable-user=<user>
   This option defines the user that simscan will run as.  By default, the
   user is 'simscan'.

--enable-clamav=y|n
   This option turns clamav scanning on or off.  When enabled, an incoming
   email will be rejected if a virus is detected in the email.

--enable-clamdscan=PATH
   This option defines the path to the clamdscan binary.  This is the full
   path to the binary.
   Note : This option does nothing when clamav is not enabled.

--enable-custom-smtp-reject=y|n
   This option turns custom smtp reject messages on and off.  When enabled
   simscan will place the virus name in the reject message if a virus is
   detected.
   Note : The qmail-queue-custom-error.patch is needed to make this option
          work properly.  You can find this patch in the contrib directory.
   Note 2 : Enabling dropmsg disables this option

--enable-per-domain=y|n
   This option turns per-domain scanning on and off.  Per domain scanning
   allows the administrator to explicitly state what scanning occurs for
   what domain.  In addition, attachment scanning can be enabled or disabled
   for each domain.  Details about how to set up per-domain scanning are
   below.

--enable-attach=y|n
   This option turns on attachment scanning.  Attachment scanning will block
   all attachments specified in /var/qmail/control/ssattach.  Attachment
   scanning is disabled by default.

--enable-spam=y|n
   Ths option turns spam scanning on and off.  When enabled, simscan allows
   mail over a certain spam threshold to be rejected back to the sender.

--enable-spam-passthru=y|n
   This option turns spam passthru on and off.  When enabled, email
   identified as spam via the X-Spam-Status: header will be passed on to the
   user instead of rejected.
   Note : Enabling spam-hits effectively disables this option.

--enable-spamc-user=y|n
   This option turns per-user spamassassin on or off.  When enabled, the email
   address of the first rcpt to user is sent to spamassassin.  This allows
   spamassassin to use customized rules and settings for that email.

--enable-spam-hits=number
   This option specifies the number of hits a spam must receive to be rejected
   by simscan.  This option defaults to 10 hits.
   Note : This option disables spam passthru

--enable-spamc=PATH
   This option specifies the full path to the spamc binary.

--enable-spamc-args=ARGS
   This option defines the arguments to pass to spamc.  Be sure to place quotes
   around the options you define.

--enable-dropmsg=y|n
   This option causes messaged to be dropped when a virus/spam is found, rather
   than returning a 5xx error to the sender.  This option is disabled by default.
   Note : This option overrides the Custom SMTP Reject option
   Note 2 : If SPAM Passthru is enabled, SPAM will NOT be dropped unless 
            spam-hits is enabled

--enable-quarantinedir=DIR
   This option defined a directory to keep spam and/or infected emails.  This
   option is disabled by default.

--enable-qmaildir=DIR
   This option defines the location of qmail.

--enable-workdir=DIR
   This option defines the location of the working directory.  Note : The
   default directory is /var/qmail/simscan

--enable-qmail-queue=PATH
   This option defines the full path and name of the qmail-queue program.
   Incoming mail is passed to this program after being scanned by SimScan.

--enable-trophie-path=PATH
   This option defines the full path to the trophie binary. This option is only
   necessary if the received line option (--enable-received) is chosen.

--enable-trophie-socket=PATH
   This option defines the path to the trophie socket.  Defining this option
   enables trophie scanning.

--enable-ripmime=PATH
   This option defines the path to the ripmime program.  This program is used
   to rip apart emails into files.

--enable-received=y|n
   Simscan adds a Received line, showing the runtime, and a version string of
   the scanners that checked the message:
   Received: by simscan 1.0.6 ppid: 25399, pid: 25400, t: 4.7007s
         scanners: attach: 1.0.6 clamav: 0.80/m:27/d:556 trophie: 7.000-1011/218/74141 spam: 2.63

--enable-spamassassin-path=PATH
   This option defines the full path to the spamassassin binary.  This option
   is only necessary if the received line option (--enable-received) is chosen.

--enable-clamavdb-path=PATH
   This option defines the full path to clamav master.cvd and daily.cvd files.
   This option is only necessary if the received line option (--enable-received)
   is chosen.

--enable-sigtool-path=PATH
   This option defines the full path to the sigtool binary.  This option is only
   necessary if the received line option (--enable-received) is chosen.
```

Attachment blocking option
------------------------

Attachments can be blocked. It is disabled by default.
If you use the per domain scanning as well, look in that section of the
documentation on how to enable the checking.
With only the attachment blocker and NO per domain option it works like
this:

Put the list of attachments in /var/qmail/control/ssattach.
List each attachment on it's own line. For example:
```
  [user@mailserver user]$ cat /var/qmail/control/ssattach
  .jpg
  .mp3
  .scr
  .bat
```

Then configure with:
```
  ./configure --enable-attach (with any other options you want)
  make
  make install-strip
```

SpamAssassin options
--------------------
There are four different ways to configure simscan with spamassassin
* no spam processing
  This is the default. No spamassassin processing will be done.

* Reject email at smtp level if spamassassin considers it spam. This might reject false positives (good email that looks like spam)
  ```--enable-spam```

* Reject email above a "score" of some number, like 15 and pass everything else through to the user.
  ```--enable-spam --enable-spam-hits=15```

* Do not reject anything. Pass the spamassassin processed message through to the user
  ```--enable-spam --enable-spam-passthru```

In addtion you can enable per user preference processing for email with just one recipient. Add the following option
  ```--enable-spamc-user```

Look at the rc.spamd sample startup script for spamd options that work with vpopmail and per user preferences. Also look at the contrib directory for patches to spamassassin to make vpopmail/per user processing work.


Trophie/Sophie option
----------------------
To enable the trophie virus scanner :
```
  ./configure --enable-trophie-socket=/path/to/the/socket \
    --enable-trophie-path=/path/to/trophie/binary
```
As sophie uses the same interface it may work as well,
but is not tested!


How SMTP rejection works
------------------------
There are currently three options for handling SMTP rejection.  SMTP
rejection occurs when a virus is detected, or when the spam score is
over the spam-hits level.

The default rejection option is to reject with a standard 5XX reject
message.  This rejection message looks as follows :
  554 mail server permanently rejected message (#5.3.0)

If --enable-dropmsg is used, messages are dropped with no rejection
message.  The connection is simply closed without warning.

If --enable-custom-smtp-reject is used, messages are rejected with
a custom message.  You will need to apply the qmail-queue-custom-error.patch
patch located in the contrib directory in order to make this work.

For virus rejection, the message contains the name of the virus such as :

  Your email was rejected because it contains the Worm.Bagle.AU virus

For spam rejection, the message is more generic, merely stating that
the message was rejected because it was considered spam :

  Your email is considered spam (53.5 spam-hits)

For attachment rejection, the message contains the name of the
attachment :

  Your email was rejected because it contains a bad attachment: trojan.exe


Enable Per Domain processing
----------------------------
To enable per domain processing :
```
  ./configure --enable-per-domain (with any other options you want)
  make
  make install-strip
```

Edit the /var/qmail/control/simcontrol text file
You can enable/disable clam/spam/trophie/attachments per domain
and per user and set a default for the whole machine.

Here is an example:
```
  [user@mailserver user]$ cat /var/qmail/control/simcontrol
  postmaster@example.com:clam=yes,spam=no,attach=.txt:.com
  example.com:clam=no,spam=yes,attach=.mp3
  :clam=yes,spam=yes,trophie=yes,spam_hits=20.1
```

The third line sets the default for the whole machine to
enable clam,trophie, spam scanning, and sets the reject level for
spam hits to 20.1.

The second line sets clam off and spam on for the example.com domain and
disallows .mp3 files for the attachment scanner

The first line sets clam on and spam off for postmaster@example.com and
.txt and .com files for the attachment names.

The order of precedence is:
```
  email address (overrides all)
  domain (overrides default)
  default (only used if not overridden by domain or email address.
```
First the sender address will be looked up and then the recipients.
Without any matches, no scans will be done.

Then run /var/qmail/bin/simscanmk to build the simcontrol.cdb file.
You can rebuild this files at any time. The simscanmk program can safely
update the cdb files while the system is running.

If you are using the --enable-received option, you also need to run simscanmk with the -g option.  This will create the simversions.cdb files that contain the current versions of spamassassin and clamav.  Freshclam, the clamav update tool, can be configured to run simscanmk -g after each update.  See the freshclam.conf file for more information.

Qmail extensions are handled like this: the address is broken into their
parts and are looked up.  test-list-owner@test.ch looks up:

```
  test@test.ch
  test-list@test.ch
  test-list-owner@test.ch
```

Security
----------
The simscan program is restricted to running setuid simscan
to protect the rest of the system. It does all of it's work
in the /var/qmail/simscan directory (default location).

The simscan program runs setuid simscan. It does all of it's work
in the /var/qmail/simscan directory which is owned by simscan.


Permissions and ClamAntiVirus
-----------------------------
To get ClamAV to play nicely with simscan's permissions you have two options:
* run clamd as root
* Add clamav to simscan's group. Then clamav will have access
to the working directory and it's files. On Linux like systems:
usermod -G simscan clamav

How to chain additional scanning programs with simscan
---------------------------
When simscan finishes it expects to call a program that reads
the file descriptors like qmail-queue does. You can configure
simscan to call a different program. By default the configure
script picks up the path to your qmail-queue program, which
is normally /var/qmail/bin/qmail-queue. Use this configure option:

```
  --enable-qmail-queue=PATH
```
where PATH is full path to qmail-queue program.

I do not know why you would want to have another scanning process happen,
but you can sure configure it before compiling.


How to Disable/Enable simscan for smtp connections by IP ranges
-------------------------------
Use the standard tcp.smtp text file to set or not set the QMAILQUEUE
environment variable per IP ranges. qmails smtp server is normally
run via tcpserver with the -x option to the constant database file
tcp.smtp.cdb.

Example:
Turn simscan off for our machines loopback address (127.*.*.*).

Disable it for hypothetical local linux users on the 192.168.1.* lan.

Enable it for hypothetical local windows users on the 192.168.2.* lan.

Finally, we enable simscan(clamav/spamassassin) for all those untrusted internet email senders.

Example tcp.smtp file contents:
```
  [user@mailserver user]$ cat tcp.smtp
  127.:allow,RELAYCLIENT=""
  192.168.1.:allow,RELAYCLIENT=""
  192.168.2.:allow,RELAYCLIENT="",QMAILQUEUE="/var/qmail/bin/simscan"
  :allow,QMAILQUEUE="/var/qmail/bin/simscan"
```

This tcp.smtp file then needs to be compiled into the tcp.smtp.cdb file
using your systems method of generating it. If you only need the rules
in the tcp.smtp file you can compile it with this command:

```
  [user@mailserver user]$ cd /to/directory/where/your/tcp.smtp.cdb file lives
  [user@mailserver user]$ /usr/local/bin/tcprules tcp.smtp.cdb tempfile < tcp.smtp
```

Once compiled, the rules take effect immediately. Actually it
takes effect on every new smtp connection.

Temporary File Management
--------------------------
Simscan uses unique file names for the message, to/from headers and the
optional spamassassin output. The files are created in the unique simscan
work directory for this process. The files are unlinked along with the
temporary work directory just before we hand/execl all the information to
qmail-queue or on error exits.

         data          file name
       ____________   _______________
       message fd 0 = msg.X.Y.Z          where X = unix seconds
       to/from fd 1 = addr.X.Y.Z               Y = microseconds
       spamassassin = spamc.msg.X.Y.Z          Z = simscan process id

fd 0 and fd 1 normally come from the qmail smtp daemon.

Temporary files and directories are not deleted if the SIMSCAN_DEBUG
environment variable is set.

Sample qmail startup script
---------------------------
rc.qmail in the contrib directory is our sample qmail startup script. It shows you how you
can set the QMAILQUEUE environment variable to call simscan for
all incoming SMTP email.

Notice that we run our smtp server as root. We also run clamd as root. If you are having permission
problems you may want to consider running both as root.

Sample spamassassin startup script
----------------------------------
rc.spamd in the contrib directory is a sample spamd startup script.
It sets enables the vpopmail and per user perferences option. It also
sets the spamd socket to /tmp/spamd.sock. 

You must use the --enable-spamc-args="-U /tmp/spamd.sock" option to
simscan if you use this startup script.
