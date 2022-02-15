# From Backup Operator To Domain Admin

If you compromise an account member of the group Backup Operators you can become the Domain Admin without RDP or WinRM on the Domain Controller.

All credit from [filip_dragovic](https://twitter.com/filip_dragovic) with his inital [POC](https://raw.githubusercontent.com/Wh04m1001/Random/main/BackupOperators.cpp) ! I build this project because I wanted to have a more generic binary with parameters and also being able to export the SAM database on the remote share ! 

```
PS C:\Users\mpgn\POC> .\BackupOperatorToDA.exe -h

Backup Operator to Domain Admin (by @mpgn_x64)

  This tool exist thanks to @filip_dragovic / https://github.com/Wh04m1001

Mandatory argument:
  -t <TARGET>      \\computer_name (ex: \\dc01.pouldard.wizard
  -o <PATH>        Where to store the sam / system / security files (can be UNC path)

Optional arguments:

  -u <USER>        Username
  -p <PASSWORD>    Password
  -d <DOMAIN>      Domain
  -h               help
```

Example: 

1. Using the user RON member of the Backup Operators group on another server than the DC
2. I dump and export the SAM database on the remote share
3. Then I read the SAM file with secretdump 
4. An I use the computer account fo the DC to dump the NTDS !

![2022-02-15_15-39](https://user-images.githubusercontent.com/5891788/154149582-42652915-57bd-436d-94b6-442aec6288b6.png)

### What's the magic ?

The code is really simple, there is only 3 steps:

1. `RegConnectRegistryA` : Establishes a connection to a predefined registry key on another computer.
2. `RegOpenKeyExA` : Opens the specified registry key
3. `RegSaveKeyA` : Saves the specified key and all of its subkeys and values to a new file

### Blackfield from HackTheBox

This box was designed by [aas_s3curity](https://twitter.com/aas_s3curity) to exploit a user from the group "Backup Operators" to become domain admin and get the root flag.
I search a little bit on the available writeups but all of them where using WinRM to exploit the "Backup Operators" group.

With this POC you don't need to have an access with WinRM or RPD :

![htb](https://user-images.githubusercontent.com/5891788/154149493-094cada5-61a5-431d-b95c-ae5be6f65ee1.PNG)

