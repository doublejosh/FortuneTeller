import React from 'react'
import { useSelector } from 'react-redux'
import { useFirebaseConnect } from 'react-redux-firebase'
import Category from './Category'

export default () => {
	useFirebaseConnect([
		{
			path: 'categories',
		},
	])
	const categories = useSelector(state => state.firebase.data.categories)

	return (
		<section className="categories">
			{categories &&
				categories.map((category, i) => (
					<Category key={category.id} {...category} />
				))}
		</section>
	)
}
