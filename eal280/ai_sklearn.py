# ai_sklearn.py - OMQ Python AI Scikit-Learn
from sklearn.linear_model import LinearRegression
print('Scikit-Learn: Erster echter AI!')
X = [[1],[2],[3],[4],[5]]
y = [10,20,30,40,50]
model = LinearRegression()
model.fit(X, y)
print('AI trainiert! Hat gelernt: 1 Stunde = 10 Punkte!')
vor = model.predict([[6]])
print(f'Vorhersage: 6 Stunden lernen = {vor[0]:.0f} Punkte!')
print('Echte AI! 3 Zeilen!')
