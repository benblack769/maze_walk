require(tidyverse)

read_out = function(file_name,type_name){
  df = read.table(file=file_name,sep='\t',header=FALSE) %>%
      select(V7) %>%
      rename(V7="Length") %>%
      mutate(Type=type_name)
}
bad_ops = read_out("out_bad_ops_gen.txt","Bad ops")
random = read_out("out_false.txt","Random")
push_pull = read_out("out_push_pull.txt","Push Pull")
push = read_out("out_push_only.txt","Push")
pull = read_out("out_pull_only.txt","Pull")
no_args = read_out("out_true_no_arg.txt","None")

data = rbind(bad_ops,random,push_pull,push,pull,no_args)

good_data = unnest(data)

data$Type = ifelse(data$V1 == 0,"random","weighted")

data$Length = data$V7

ggplot(data,aes(x=factor(Type),y=Length))+
  geom_boxplot() + 
  scale_y_log10() 
