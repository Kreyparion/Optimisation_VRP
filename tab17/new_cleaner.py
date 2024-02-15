import pandas as pd
import numpy as np
import re 

vehicule_path = "vehicules.csv"

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
    
df.loc[:, df.columns != 'parameters_name'] = df.loc[:, df.columns != 'parameters_name'].applymap(extraire_nombre)

print(df)
#on ré-écrit le csv

df.to_csv('vehicule_cleaned.csv', sep='\t', index=False)