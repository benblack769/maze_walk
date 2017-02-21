require(tidyverse)

read_out = function(file_name,type_name){
  df = read.table(file=file_name,sep='\t',header=FALSE) %>%
      select(V7) %>%
      rename(Length=V7) %>%
      mutate(Type=type_name)
}
bad_ops = read_out("out_bad_ops_gen.txt","Bad ops")
random = read_out("out_false.txt","Random")
push_pull = read_out("out_push_pull.txt","Push Pull")
push = read_out("out_push_only.txt","Push")
pull = read_out("out_pull_only.txt","Pull")
no_args = read_out("out_true_no_arg.txt","None")

data = rbind(bad_ops,random,push_pull,push,pull,no_args)

<<<<<<< Updated upstream
data$Type = factor(data$Type)
=======
data$Type = ifelse(data$V1 == 0,"random",
            ifelse(data$V3 == 0.15,
                ifelse(data$V4 == -1.5, "push_pull", "push"),
                ifelse(data$V4 == -1.5, "pull","weighted")))
>>>>>>> Stashed changes


<<<<<<< Updated upstream

ggplot(data,aes(x=reorder(Type, Length, FUN=median),y=Length))+
  geom_boxplot() + 
  scale_y_log10() 

=======
ggplot(data,aes(x=factor(Type),y=Length))+
  geom_boxplot() +
  scale_y_log10()
>>>>>>> Stashed changes
