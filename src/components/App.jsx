import React from 'react'
import { BrowserRouter as Router, Switch, Route } from 'react-router-dom'
import Logo from './Logo'
import Categories from './Dashboard/Categories'
import Links from './Links'
import Vote from './Survey/Vote'
import Add from './Survey/Add'
import Results from './Survey/Results'
import Test from './Test'
import Menu from './Menu'
import '../style.scss'

export default () => (
	<Router>
		<Switch>
			<Route path="/vote">
				{/* <Logo css={{ '& h1': { opacity: '.2', width: '100%', textAlign: 'center' } }} /> */}
				<Vote />
				<Menu />
			</Route>
			<Route path="/results">
				<Results />
				<Links />
				<Menu />
			</Route>
			<Route path="/add">
				<Add />
				<Menu />
			</Route>
			<Route path="/test">
				<Test />
				<Menu />
			</Route>
			<Route path="/">
				<Logo />
				<Categories />
				<Links />
				<Menu />
			</Route>
		</Switch>
	</Router>
)
