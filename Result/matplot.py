import matplotlib.pyplot as plt

# Using 4 ranks
local_sums = []
world_size = 4 

for rank in range(world_size):
    with open(f"log_rank_{rank}.txt", "r") as file:
        line = file.readline()
        local_sum = int(line.split()[-1])
        local_sums.append(local_sum)

plt.bar(range(world_size), local_sums)
plt.xlabel('Process Rank')
plt.ylabel('Local Sum')
plt.title('Local Sums by Process')

plt.xticks(range(world_size))

# Display local sum values ​​on the bar chart
for i, v in enumerate(local_sums):
    plt.text(i, v + 1, str(v), ha='center', va='bottom')

plt.show()