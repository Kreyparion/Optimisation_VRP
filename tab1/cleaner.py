import pandas as pd
import numpy as np
import re 

vehicule_path = "tab1/vehicules.csv"
coordinates_path = "tab1/coordinates.csv"

df_coord = pd.read_csv(coordinates_path, delimiter='\t')

df = pd.read_csv(vehicule_path, delimiter='\t')

df.rename(columns={"Unnamed: 0": "parameters_name"}, inplace=True)

# Fonction pour extraire les nombres des chaînes de caractères
def extraire_nombre(val):
    if pd.isnull(val):
        return val  # Garder NaN tel quel
    # Utiliser une expression régulière pour trouver les nombres
    match = re.findall(r'\b\d+\.?\d*\b', str(val))
    if match:
        # Convertir en float si un nombre est trouvé
        return float(match[0])
    else:
        # Retourner NaN si aucun nombre n'est trouvé
        return "-"
    
df.loc[:, df.columns != 'Unnamed: 0'] = df.loc[:, df.columns != 'Unnamed: 0'].applymap(extraire_nombre)

if 'Coordinates' in df_coord.columns:
    cols_to_transform = df_coord.columns[df_coord.columns != 'Coordinates']
    for col in cols_to_transform:
        df_coord[col] = df_coord[col].apply(extraire_nombre)
print(df_coord)
print(df)
#on ré-écrit le csv

df.to_csv('tab1/vehicule_cleaned.csv', sep='\t', index=False)

df_coord.to_csv('tab1/coordinates_cleaned.csv', sep='\t', index=False)

