import React from 'react'
import { useSelector } from 'react-redux'
import { useFirebaseConnect } from 'react-redux-firebase'
import Category from './Category'
import Stats from './Stats'

export default () => {
	useFirebaseConnect(['categories', 'fortunes'])
	const categories = useSelector(state => state.firebase.data.categories)
	const fortunes = useSelector(state => state.firebase.data.fortunes)

	return (
		<React.Fragment>
			<Stats fortunes={fortunes} />
			<section className="categories">
				{categories &&
					categories.map((category, i) => {
						return (
							<Category
								key={category.id}
								{...category}
								fortunes={fortunes}
							/>
						)
					})}
			</section>
		</React.Fragment>
	)
}
