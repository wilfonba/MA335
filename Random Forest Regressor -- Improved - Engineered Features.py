#!/usr/bin/env python
# coding: utf-8

# In[1]:


# obtain imports
import graphviz 
import pandas as pd
import numpy as np

from sklearn.model_selection import train_test_split, GridSearchCV, cross_validate
from sklearn.ensemble import RandomForestRegressor
from sklearn.model_selection import GridSearchCV
from sklearn.tree import export_graphviz
import matplotlib.pyplot as plt
import graphviz

from sklearn.metrics import mean_absolute_error, mean_squared_error


# In[2]:


# get the train sets
X_train = pd.read_csv('Xe_train.csv')
y_train = pd.read_csv('y_train.csv')


# In[ ]:


# determine the number of estimators

A = np.arange(1, 15, 5)
B = np.arange(10, 200, 10)
grid = {'max_depth':A, 'n_estimators':B}

# make random forest regressor
rf = RandomForestRegressor()
rfCV=GridSearchCV(rf, param_grid=grid, return_train_score=True, n_jobs=-1)
rfCV.fit(X_train, y_train.values.ravel())

# print the results
print('best B', rfCV.best_params_)
print('valid R2', rfCV.best_score_.round(3))

# put results in a data frame
results = pd.DataFrame()
results['trees']= grid['n_estimators']
results['train R2'] = rfCV.cv_results_['mean_train_score']
results['valid R2'] = rfCV.cv_results_['mean_test_score']

# plot the results
ax1 = results.plot.line(x='trees', y='train R2')
results.plot.line(x='trees', y='valid R2', ax=ax1)


# In[ ]:


# cross validation of results
cv_results = cross_validate(rf, X_train, y_train.values.ravel(), return_train_score=True)
R2_trainCV = cv_results['train_score'].mean()
R2_validCV = cv_results['test_score'].mean()
print('train R2 (CV)', R2_trainCV.round(3))
print('valid R2 (CV)', R2_validCV.round(3))


# In[ ]:


# quick look at feature importance
feature_importance = pd.Series(rf.feature_importances_, index=X_train.columns)
feature_importance.sort_values(ascending=False)

