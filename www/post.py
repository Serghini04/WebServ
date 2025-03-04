import sys

def main():
    # Read from stdin
    input_data = sys.stdin.read()
    
    # Print to stdout
    print(input_data, end="")

    # Infinite loop (potentially dangerous)
    while True:
        print("fff")

if __name__ == "__main__":
    main()
