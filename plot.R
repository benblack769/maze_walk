require(tidyverse)

file_name = "out_all_fin.txt"

df = read.table(file=file_name,sep='\t',header=TRUE)
df$walk_name = factor(df$walk_name)

ggplot(df,aes(x=walk_name,y=tot_dis))+
  geom_boxplot() + 
  scale_y_log10() 
