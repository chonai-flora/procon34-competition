import os
import csv
import math
import json
import random


# フィールドを選択
def choose_field(csv_filename):
    with open(csv_filename) as f:
        reader = csv.reader(f)
        grid = [line for line in reader]
        structures = []
        masons = []
        mason_count = {"a": 0, "b": 0}

        for line in grid:
            tmp_structures = []
            tmp_masons = []

            for cell in line:
                if cell.isdigit():
                    tmp_structures.append(int(cell))
                    tmp_masons.append(0)
                else:
                    if cell == "a":
                        mason_count["a"] += 1
                    else:
                        mason_count["b"] -= 1
                        
                    tmp_structures.append(0)
                    tmp_masons.append(mason_count[cell])

            structures.append(tmp_structures)
            masons.append(tmp_masons)

        return {
            "mason": mason_count["a"],
            "structures": structures,
            "masons": masons
        }

    return None


# JSONデータを生成
def generate_json(csv_path, csv_filename):
    path = "n-jo-tanijo/App/server/json/"
    match_id = sum(os.path.isfile(os.path.join(path, name)) for name in os.listdir(path))
    
    field = choose_field(csv_path + csv_filename)
    size = len(field["structures"])
    max_turn = math.floor((size - 8) * 1.2) * 10
    turn_seconds = min((size - 11) // 2 + 5, 15)

    data = {
        "teams": [
            {"name": "teamA", "token": "0"},
            {"name": "teamB", "token": "1"}
        ],
        "match": {
            "id": match_id + 11,
            "turns": max_turn,
            "turnSeconds": turn_seconds,
            "bonus": {
                "wall": 10,
                "territory": 30,
                "castle": 100
            },
            "board": {
                "width": size,
                "height": size,
                "mason": field["mason"],
                "structures": field["structures"],
                "masons": field["masons"]
            }
        }
    }

    # JSONデータを書き出し
    filename = path + csv_filename.replace(".csv", ".json")
    with open(filename, mode="wt", encoding="utf-8") as f:
	    json.dump(data, f, ensure_ascii=False, indent=4)
	    print(f'"{csv_path + csv_filename}" から "{filename}" を作成しました')
        

if __name__ == "__main__":
    path = "fields/"
    filenames = list(filter(lambda name: name[3:] == ".csv", os.listdir(path)))
    for filename in filenames:
        generate_json(path, filename)