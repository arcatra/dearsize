DearSize is an Open Source Linux tool, that can tell you the exact size and metadata about a directory.
DearSize is free to use tool and can be used by anyone who has Linux installed, as long as you follow the rules and regulations of this project's GNU GENERAL PUBLIC LICENSE which you find in this repository.

> [!Note]
> -> Dearsize includes the sizes of each and every unreadable/unreachable directories, and sym links when calculating the 
total size of the source directory. which is about 4096 Bytes for each(sym link, and unreadable DIRs) or around 4.1KB each.
> -> The reason for this is because I wanted this tool to be precise.
> -> And when you think about the result, if you have around 100 sym links, 60 unreadable DIRs in the source dir,
then the resulting total Bytes would be (100 + 60) * 4096 = 655360 Bytes(665.36 KB) for all of the sym links, 
and the unreadable DIRs alone. And this would eventually affect the other metrics like KB, MB, GB.., which are calculated based on Bytes.

>  In the feature updates I will add a flag, which you can use to tell dearsize to not consider the sym links, and unreadable directories sizes during the calculation.
