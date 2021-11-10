# Installation

- [Install Git](https://git-scm.com/download/win) on your computer and pull the repo into your local machine.

- Pull from this repository using `git`
```
  git clone https://github.com/rawsashimi1604/Chatbot_Project.git

```

- From there you should be good to go to start coding!

- To add to the codebase use Git! `git add .` adds all changes, `git status` checks status of Git, `git commit -m ...` adds a message to your commit for better referencing, `git push origin master` pushes your code to our master branch! To add individual files, use `git add <file_name>`

```
  git add .
  git status
  git commit -m "<enter your message here>"
  git push origin master
```

- To pull from existing codebase (get our latest updates), this will pull anything from master into your current master branch.

```
  git pull
  git merge origin/master
```

## To Do List
- First, make sure that you understand the skeleton code 
- `chatbot_is_smalltalk()` is to read,  (Jun Hao) and `chatbot_do_smalltalk()` is to reply (Jun Hao) 
- Implement a preliminary version of `knowledge_get()` (Getting the ini file) that simply returns a few hard-coded responses to questions, 
and implement `chatbot_is_question()` (Anne) and  `chatbot_do_question()` to use it, if answer is null or not found inside knowledge, ask for answer, update the knowledge based on answer. 
- Implement `Knowledge_put()`, refresh `knowledge_get()` (Gavin Kor Kor) 
- `chatbot_do_reset()` so that it erases all of the knowledge created by `knowledge_put()` (Kun)
- `chatbot_do_reset()` also resets when u relaunch the program. (Kun) 
- Implement `chatbot_do_save()` (Saves the ini)  (Shi En) and `chatbot_do_load()` (Load previous existing ini)  (Shi En) 

## Deadline
- **10** (Understand the skeleton code)
- **10 - 13** (Week 10) (Attempt coding and see our progress, If need can hold meeting. Sat (Mid morning before lunch) preferred) 
- **13th** (Week 10 end) 
- **16 - 23** Nov (Week 11)  (Working function, able to individually work for individual functions, Combine code) 
- **23 - 25** Nov (Week 12) (Video, Report and Code) 
- **2nd** Dec (Final Deadline/Buffer) 
