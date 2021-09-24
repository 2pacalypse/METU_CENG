module HW1 where

--Data
------

type RealName = String
type UserName = String
type GroupName = String
type Message = String

data Post    = Post UserName Message deriving (Show, Eq)
data To 	 = UserID UserName | GroupID GroupName deriving (Show, Eq)
data User    = User UserName RealName [UserName] [Post] deriving (Show, Eq)
data Group   = Group GroupName [UserName] [Post] deriving (Show, Eq)
data DB		 = DB [User] [Group] deriving (Show, Eq)

--1. Commands

newUser      :: DB -> User -> DB
addFriend    :: DB -> UserName -> UserName -> DB
sendPost 	 :: DB -> UserName -> Message -> [To] -> DB
newGroup 	 :: DB -> GroupName -> DB
addMember 	 :: DB -> GroupName -> UserName -> DB
removeMember :: DB -> GroupName -> UserName -> DB

--2. Queries

getFriendNames :: DB -> UserName -> [RealName]
getPosts 	   :: DB -> To -> [Post]
listGroups 	   :: DB -> UserName -> [Group]
suggestFriends :: DB -> User -> Int -> [User]

---- IMPLEMENTATIONS ----

newUser db@(DB users groups) user = if not (elem user users) then DB (user:users) (groups) else db

addFriend (DB users groups) u1 u2  = DB 
	[case ()
		 of _
			| (username == u1) -> User username realname (u2:friends) posts
			| (username == u2) -> User username realname (u1:friends) posts
			| (otherwise) -> x
	|x@(User username realname friends posts)<- users] groups


sendPost (DB users groups) user message recipients = DB 
	

	[if (elem (UserID username) recipients|| elem (username) getGroupMembers)
		then User username realname friends (newpost:uposts)
		else x  
	| x@(User username realname friends uposts) <- users]

	[if (elem (GroupID groupname) recipients)
		then Group groupname members (newpost:posts)
		else x
	| x@(Group groupname members posts) <- groups]

	where newpost = (Post user message)
	      getGroupMembers =  [m | x@(Group g ms ps) <- groups, m<- ms, elem (GroupID g) recipients]


newGroup (DB users groups) groupname = DB users (newgroup:groups)
	where newgroup = Group groupname [] []

addMember (DB users groups) gname uname = DB users
	[if(groupname == gname && not (elem uname members))
		then Group groupname (uname:members) posts
		else x
	| x@(Group groupname members posts) <- groups]



removeMember (DB users groups) groupname membername = DB users
	[if (gname == groupname)
		then Group gname [a | a <- members, a /= membername] posts
		else x
	| x@(Group gname members posts) <- groups]



getFriendNames (DB users groups) user = 
	[realname | x@(User username realname friends posts) <- users, elem user friends]




listGroups (DB users groups) user = [x | x@(Group gname members posts)<- groups, elem user members]


getPosts (DB ((User username _ _ posts):users) groups) (UserID u) = 
	if (u == username) then posts
			   else getPosts(DB users groups) (UserID u) 

getPosts (DB users ((Group gname _  posts):groups)) (GroupID g) = 
	if (g == gname) then posts
			else getPosts(DB users groups) (GroupID g)

suggestFriends (DB users groups) (User username _ friends _) n =
	[x | x@(User u r fs ps) <- users,let weight = foldl (\acc y -> if(elem y friends) then acc + 1 else acc) 0 fs, weight >= n, u /= username, notElem u friends]
