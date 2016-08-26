设置全局用户信息
$ git config --global user.name "Your Name"
$ git config --global user.email "email@example.com"

所以，创建一个版本库非常简单，首先，选择一个合适的地方，创建一个空目录：
$ mkdir tomgit
$ cd tomgit
$ pwd
/d/tomgit

第二步，通过git init命令把这个目录变成Git可以管理的仓库：
$ git init
Initialized empty Git repository in /Users/michael/learngit/.git/
言归正传，现在我们编写一个readme.txt文件，内容如下：
Git is a version control system.
Git is free software.

第一步，用命令git add告诉Git，把文件添加到仓库：
$ git add readme.txt
第二步，用命令git commit告诉Git，把文件提交到仓库：
git commit -m "wrote a readme file"
时光机穿梭

阅读: 525898
我们已经成功地添加并提交了一个readme.txt文件，现在，是时候继续工作了，于是，我们继续修改readme.txt文件，改成如下内容：
Git is a distributed version control system.
Git is free software.
现在，运行git status命令看看结果：
虽然Git告诉我们readme.txt被修改了，但如果能看看具体修改了什么内容，自然是很好的。比如你休假两周从国外回来，第一天上班时，已经记不清上次怎么修改的readme.txt，所以，需要用git diff这个命令看看：
git diff readme.txt 
################
--当然了，在实际工作中，我们脑子里怎么可能记得一个几千行的文件每次都改了什么内容，不然要版本控制系统干什么。版本控制系统肯定有某个命令可以告诉我们历史记录，在Git中，我们用git log命令查看：
git log
git log命令显示从最近到最远的提交日志，我们可以看到3次提交，最近的一次是append GPL，上一次是add distributed，最早的一次是wrote a readme file。
如果嫌输出信息太多，看得眼花缭乱的，可以试试加上--pretty=oneline参数：
$ git log --pretty=oneline
版本回退
$ git reset --hard HEAD^
git reset --hard 3628164
版本回退

阅读: 578444
现在，你已经学会了修改文件，然后把修改提交到Git版本库，现在，再练习一次，修改readme.txt文件如下：

Git is a distributed version control system.
Git is free software distributed under the GPL.
然后尝试提交：

$ git add readme.txt
$ git commit -m "append GPL"
[master 3628164] append GPL
 1 file changed, 1 insertion(+), 1 deletion(-)
 像这样，你不断对文件进行修改，然后不断提交修改到版本库里，就好比玩RPG游戏时，每通过一关就会自动把游戏状态存盘，如果某一关没过去，你还可以选择读取前一关的状态。有些时候，在打Boss之前，你会手动存盘，以便万一打Boss失败了，可以从最近的地方重新开始。Git也是一样，每当你觉得文件修改到一定程度的时候，就可以“保存一个快照”，这个快照在Git中被称为commit。一旦你把文件改乱了，或者误删了文件，还可以从最近的一个commit恢复，然后继续工作，而不是把几个月的工作成果全部丢失。

 现在，我们回顾一下readme.txt文件一共有几个版本被提交到Git仓库里了：

 版本1：wrote a readme file

 Git is a version control system.
 Git is free software.
 版本2：add distributed

 Git is a distributed version control system.
 Git is free software.
 版本3：append GPL

 Git is a distributed version control system.
 Git is free software distributed under the GPL.
 当然了，在实际工作中，我们脑子里怎么可能记得一个几千行的文件每次都改了什么内容，不然要版本控制系统干什么。版本控制系统肯定有某个命令可以告诉我们历史记录，在Git中，我们用git log命令查看：

 $ git log
 commit 3628164fb26d48395383f8f31179f24e0882e1e0
 Author: Michael Liao <askxuefeng@gmail.com>
 Date:   Tue Aug 20 15:11:49 2013 +0800

     append GPL

     commit ea34578d5496d7dd233c827ed32a8cd576c5ee85
     Author: Michael Liao <askxuefeng@gmail.com>
     Date:   Tue Aug 20 14:53:12 2013 +0800

         add distributed

	 commit cb926e7ea50ad11b8f9e909c05226233bf755030
	 Author: Michael Liao <askxuefeng@gmail.com>
	 Date:   Mon Aug 19 17:51:55 2013 +0800

	     wrote a readme file
	     git log命令显示从最近到最远的提交日志，我们可以看到3次提交，最近的一次是append GPL，上一次是add distributed，最早的一次是wrote a readme file。
	     如果嫌输出信息太多，看得眼花缭乱的，可以试试加上--pretty=oneline参数：

	     $ git log --pretty=oneline
	     3628164fb26d48395383f8f31179f24e0882e1e0 append GPL
	     ea34578d5496d7dd233c827ed32a8cd576c5ee85 add distributed
	     cb926e7ea50ad11b8f9e909c05226233bf755030 wrote a readme file
	     需要友情提示的是，你看到的一大串类似3628164...882e1e0的是commit id（版本号），和SVN不一样，Git的commit id不是1，2，3……递增的数字，而是一个SHA1计算出来的一个非常大的数字，用十六进制表示，而且你看到的commit id和我的肯定不一样，以你自己的为准。为什么commit id需要用这么一大串数字表示呢？因为Git是分布式的版本控制系统，后面我们还要研究多人在同一个版本库里工作，如果大家都用1，2，3……作为版本号，那肯定就冲突了。

	     每提交一个新版本，实际上Git就会把它们自动串成一条时间线。如果使用可视化工具查看Git历史，就可以更清楚地看到提交历史的时间线：

	     git-log-timeline

	     好了，现在我们启动时光穿梭机，准备把readme.txt回退到上一个版本，也就是“add distributed”的那个版本，怎么做呢？

	     首先，Git必须知道当前版本是哪个版本，在Git中，用HEAD表示当前版本，也就是最新的提交3628164...882e1e0（注意我的提交ID和你的肯定不一样），上一个版本就是HEAD^，上上一个版本就是HEAD^^，当然往上100个版本写100个^比较容易数不过来，所以写成HEAD~100。

	     现在，我们要把当前版本“append GPL”回退到上一个版本“add distributed”，就可以使用git reset命令：

	     $ git reset --hard HEAD^
	     HEAD is now at ea34578 add distributed
	     --hard参数有啥意义？这个后面再讲，现在你先放心使用。

	     看看readme.txt的内容是不是版本add distributed：

	     $ cat readme.txt
	     Git is a distributed version control system.
	     Git is free software.
	     果然。

	     还可以继续回退到上一个版本wrote a readme file，不过且慢，然我们用git log再看看现在版本库的状态：

	     $ git log
	     commit ea34578d5496d7dd233c827ed32a8cd576c5ee85
	     Author: Michael Liao <askxuefeng@gmail.com>
	     Date:   Tue Aug 20 14:53:12 2013 +0800

	         add distributed

		 commit cb926e7ea50ad11b8f9e909c05226233bf755030
		 Author: Michael Liao <askxuefeng@gmail.com>
		 Date:   Mon Aug 19 17:51:55 2013 +0800

		     wrote a readme file
		     最新的那个版本append GPL已经看不到了！好比你从21世纪坐时光穿梭机来到了19世纪，想再回去已经回不去了，肿么办？

		     办法其实还是有的，只要上面的命令行窗口还没有被关掉，你就可以顺着往上找啊找啊，找到那个append GPL的commit id是3628164...，于是就可以指定回到未来的某个版本：

		     $ git reset --hard 3628164
		     HEAD is now at 3628164 append GPL
		     版本号没必要写全，前几位就可以了，Git会自动去找。当然也不能只写前一两位，因为Git可能会找到多个版本号，就无法确定是哪一个了。

		     再小心翼翼地看看readme.txt的内容：

		     $ cat readme.txt
		     Git is a distributed version control system.
		     Git is free software distributed under the GPL.
		     果然，我胡汉三又回来了。

		     Git的版本回退速度非常快，因为Git在内部有个指向当前版本的HEAD指针，当你回退版本的时候，Git仅仅是把HEAD从指向append GPL：

		     git-head

		     改为指向add distributed：

		     git-head-move

		     然后顺便把工作区的文件更新了。所以你让HEAD指向哪个版本号，你就把当前版本定位在哪。
		     现在，你回退到了某个版本，关掉了电脑，第二天早上就后悔了，想恢复到新版本怎么办？找不到新版本的commit id怎么办？

		     在Git中，总是有后悔药可以吃的。当你用$ git reset --hard HEAD^回退到add distributed版本时，再想恢复到append GPL，就必须找到append GPL的commit id。Git提供了一个命令git reflog用来记录你的每一次命令：
		     git reflog

		     提交后，用git diff HEAD -- readme.txt命令可以查看工作区和版本库里面最新版本的区别：
		      git checkout -- readme.txt
		      git checkout -- file命令中的--很重要，没有--，就变成了“切换到另一个分支”的命令，我们在后面的分支管理中会再次遇到git checkout命令

		      现在你有两个选择，一是确实要从版本库中删除该文件，那就用命令git rm删掉，并且git commit：
		      远程仓库

		      阅读: 429372
		      到目前为止，我们已经掌握了如何在Git仓库里对一个文件进行时光穿梭，你再也不用担心文件备份或者丢失的问题了。

		      可是有用过集中式版本控制系统SVN的童鞋会站出来说，这些功能在SVN里早就有了，没看出Git有什么特别的地方。

		      没错，如果只是在一个仓库里管理文件历史，Git和SVN真没啥区别。为了保证你现在所学的Git物超所值，将来绝对不会后悔，同时为了打击已经不幸学了SVN的童鞋，本章开始介绍Git的杀手级功能之一（注意是之一，也就是后面还有之二，之三……）：远程仓库。

		      Git是分布式版本控制系统，同一个Git仓库，可以分布到不同的机器上。怎么分布呢？最早，肯定只有一台机器有一个原始版本库，此后，别的机器可以“克隆”这个原始版本库，而且每台机器的版本库其实都是一样的，并没有主次之分。

		      你肯定会想，至少需要两台机器才能玩远程库不是？但是我只有一台电脑，怎么玩？

		      其实一台电脑上也是可以克隆多个版本库的，只要不在同一个目录下。不过，现实生活中是不会有人这么傻的在一台电脑上搞几个远程库玩，因为一台电脑上搞几个远程库完全没有意义，而且硬盘挂了会导致所有库都挂掉，所以我也不告诉你在一台电脑上怎么克隆多个仓库。

		      实际情况往往是这样，找一台电脑充当服务器的角色，每天24小时开机，其他每个人都从这个“服务器”仓库克隆一份到自己的电脑上，并且各自把各自的提交推送到服务器仓库里，也从服务器仓库中拉取别人的提交。

		      完全可以自己搭建一台运行Git的服务器，不过现阶段，为了学Git先搭个服务器绝对是小题大作。好在这个世界上有个叫GitHub的神奇的网站，从名字就可以看出，这个网站就是提供Git仓库托管服务的，所以，只要注册一个GitHub账号，就可以免费获得Git远程仓库。

		      在继续阅读后续内容前，请自行注册GitHub账号。由于你的本地Git仓库和GitHub仓库之间的传输是通过SSH加密的，所以，需要一点设置：

		      第1步：创建SSH Key。在用户主目录下，看看有没有.ssh目录，如果有，再看看这个目录下有没有id_rsa和id_rsa.pub这两个文件，如果已经有了，可直接跳到下一步。如果没有，打开Shell（Windows下打开Git Bash），创建SSH Key：
		      $ ssh-keygen -t rsa -C "youremail@example.com"
		      现在，我们根据GitHub的提示，在本地的learngit仓库下运行命令：

$ git remote add origin git@github.com:tomcomein1/tomgit.git
下一步，就可以把本地库的所有内容推送到远程库上：
$ git push -u origin master
$ git push origin master
要关联一个远程库，使用命令git remote add origin git@server-name:path/repo-name.git；
关联后，使用命令git push -u origin master第一次推送master分支的所有内容；
此后，每次本地提交后，只要有必要，就可以使用命令git push origin master推送最新修改；


