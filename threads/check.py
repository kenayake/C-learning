sum = 1000

for i in range(4):
  with open(f"threads/transactions{i+1}.txt", "r") as f:
    for line in f:
      trx = line.split(" ")
      action = trx[0]
      amt = int(trx[1])
      if (action == "withdraw"):
        sum -= amt
      else: 
        sum += amt

print(sum)